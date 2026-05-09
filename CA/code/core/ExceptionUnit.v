
`timescale 1ns / 1ps

module ExceptionUnit(
    input              clk,
    input              rst,

    // CSR instruction info from MEM stage
    input              csr_rw_in,
    input      [1:0]   csr_wsc_mode_in,   // funct3[1:0]: 01=rw, 10=rs, 11=rc
    input              csr_w_imm_mux,     // 0: rs1 data, 1: zimm
    input      [11:0]  csr_rw_addr_in,
    input      [31:0]  csr_w_data_reg,
    input      [4:0]   csr_w_data_imm,
    output reg [31:0]  csr_r_data_out,

    // exception / interrupt from WB stage
    input              interrupt,
    input              illegal_inst,
    input              ecall_m,
    input              l_access_fault,
    input              s_access_fault,

    // mret from MEM stage
    input              mret,

    // PC info
    input      [31:0]  epc_cur,   // current instruction PC (for exception)
    input      [31:0]  epc_next,  // next instruction PC (for interrupt)

    // redirect control
    output reg [31:0]  PC_redirect,
    output reg         redirect_mux,

    // pipeline flush
    output wire        reg_FD_flush,
    output wire        reg_DE_flush,
    output wire        reg_EM_flush,
    output wire        reg_MW_flush,

    // cancel register writeback for faulting instruction
    output wire        RegWrite_cancel
);

    // ----------------------------
    // CSR registers (only M-mode)
    // ----------------------------
    reg [31:0] mstatus;
    reg [31:0] mtvec;
    reg [31:0] mepc;
    reg [31:0] mcause;

    // temp regs for state machine
    reg [31:0] epc_reg;
    reg [31:0] cause_reg;

    // CSR addresses
    localparam CSR_MSTATUS = 12'h300;
    localparam CSR_MTVEC   = 12'h305;
    localparam CSR_MEPC    = 12'h341;
    localparam CSR_MCAUSE  = 12'h342;

    // state machine
    localparam STATE_IDLE   = 3'd0;
    localparam STATE_MEPC   = 3'd1;
    localparam STATE_MCAUSE = 3'd2;

    reg [2:0] state, next_state;

    wire trap_req_wb;
    wire sync_exception;

    assign trap_req_wb    = interrupt | illegal_inst | ecall_m | l_access_fault | s_access_fault;
    assign sync_exception = illegal_inst | ecall_m | l_access_fault | s_access_fault;

    // ----------------------------
    // CSR read mux     按地址选择输出哪个 CSR 的值
    // ----------------------------
    always @(*) begin
        case (csr_rw_addr_in)
            CSR_MSTATUS: csr_r_data_out = mstatus;
            CSR_MTVEC  : csr_r_data_out = mtvec;
            CSR_MEPC   : csr_r_data_out = mepc;
            CSR_MCAUSE : csr_r_data_out = mcause;
            default    : csr_r_data_out = 32'h00000000;
        endcase
    end

    // ----------------------------
    // next-state logic     IDLE → (trap) → MEPC → MCAUSE → IDLE
    //  根据当前状态state和trap_req_wb决定FSM状态机下一状态
    // ----------------------------
    always @(*) begin
        next_state = state;
        case (state)
            STATE_IDLE: begin
                if (trap_req_wb)
                    next_state = STATE_MEPC;
                else
                    next_state = STATE_IDLE;
            end

            STATE_MEPC: begin
                next_state = STATE_MCAUSE;
            end

            STATE_MCAUSE: begin
                next_state = STATE_IDLE;
            end

            default: begin
                next_state = STATE_IDLE;
            end
        endcase
    end

    // ----------------------------
    // redirect control 
    // trap: in STATE_MEPC, set next cycle's PC to mtvec
    // mret: in STATE_IDLE, set next cycle's PC to mepc
    // ----------------------------
    always @(*) begin
        redirect_mux = 1'b0;    //PC+4
        PC_redirect  = 32'h00000000;

        // trap: 在STATE_MEPC时，下一拍重定向到mtvec
        if (state == STATE_MEPC) begin
            redirect_mux = 1'b1;    //PC_redirect
            PC_redirect  = {mtvec[31:2], 2'b00};    //PC要跳转到trap handler 入口
        end
        // mret: 仅在STATE_IDLE且mret=1时，下一拍重定向到mepc
        else if (state == STATE_IDLE && mret) begin
            redirect_mux = 1'b1;
            PC_redirect  = mepc;
        end
    end

    // ----------------------------
    // flush control
    // 遇到trap或是返回原程序时FD/DE/EM需要flush
    // ----------------------------
    assign reg_FD_flush = (state == STATE_MEPC) | (state == STATE_IDLE && mret);
    assign reg_DE_flush = (state == STATE_MEPC) | (state == STATE_IDLE && mret);
    assign reg_EM_flush = (state == STATE_MEPC) | (state == STATE_IDLE && mret);
    assign reg_MW_flush = 1'b0;

    // cancel WB only for synchronous exception when entering exception flow
    assign RegWrite_cancel = (state == STATE_MEPC) & sync_exception;

    // helpers
    reg [31:0] csr_old;
    reg [31:0] csr_wdata;
    reg [31:0] csr_src;

    // ----------------------------
    // sequential logic
    // ----------------------------
    always @(posedge clk or posedge rst) begin
        if (rst) begin
            state    <= STATE_IDLE;
            mstatus  <= 32'h0000_0008;  //MIE=1
            mtvec    <= 32'h0000_0000;
            mepc     <= 32'h0000_0000;
            mcause   <= 32'h0000_0000;
            epc_reg  <= 32'h0000_0000;
            cause_reg<= 32'h0000_0000;
        end
        else begin
            state <= next_state;

            case (state)
                // ----------------------------
                // idle:
                // - normal CSR write
                // - latch trap info
                // - restore mstatus for mret
                // ----------------------------
                STATE_IDLE: begin
                    if (trap_req_wb) begin
                        // record epc
                        // For interrupt: epc_next is the PC to return to
                        // For exception: epc_cur is the faulting instruction PC
                        if (interrupt)
                            epc_reg <= epc_next;    // 中断：下一条的 PC
                        else
                            epc_reg <= epc_cur;     //异常：当前指令的 PC

                        // record cause
                        if (interrupt)
                            cause_reg <= 32'h8000_000b; //Machine external interrupt
                        else if (illegal_inst)
                            cause_reg <= 32'd2;
                        else if (ecall_m)
                            cause_reg <= 32'd11;
                        else if (l_access_fault)
                            cause_reg <= 32'd5;
                        else if (s_access_fault)
                            cause_reg <= 32'd7;

                        // update mstatus: MPIE <= MIE, MIE <= 0
                        mstatus[7] <= mstatus[3];
                        mstatus[3] <= 1'b0;
                    end
                    else if (mret) begin
                        // restore mstatus: MIE <= MPIE, MPIE <= 1
                        mstatus[3] <= mstatus[7];
                        mstatus[7] <= 1'b1;
                    end
                    else if (csr_rw_in) begin   //正常CSR指令
                        csr_src = csr_w_imm_mux ? {27'b0, csr_w_data_imm} : csr_w_data_reg;

                        case (csr_rw_addr_in)   //按 CSR 地址选择"操作哪一个 CSR 寄存器"，相当于一个地址译码器
                            CSR_MSTATUS: begin
                                csr_old = mstatus;
                                case (csr_wsc_mode_in)
                                    2'b01: csr_wdata = csr_src;
                                    2'b10: csr_wdata = csr_old | csr_src;
                                    2'b11: csr_wdata = csr_old & ~csr_src;
                                    default: csr_wdata = csr_old;
                                endcase
                                mstatus <= csr_wdata;
                            end

                            CSR_MTVEC: begin
                                csr_old = mtvec;
                                case (csr_wsc_mode_in)
                                    2'b01: csr_wdata = csr_src;
                                    2'b10: csr_wdata = csr_old | csr_src;
                                    2'b11: csr_wdata = csr_old & ~csr_src;
                                    default: csr_wdata = csr_old;
                                endcase
                                mtvec <= csr_wdata;
                            end

                            CSR_MEPC: begin
                                csr_old = mepc;
                                case (csr_wsc_mode_in)
                                    2'b01: csr_wdata = csr_src;
                                    2'b10: csr_wdata = csr_old | csr_src;
                                    2'b11: csr_wdata = csr_old & ~csr_src;
                                    default: csr_wdata = csr_old;
                                endcase
                                mepc <= csr_wdata;
                            end

                            CSR_MCAUSE: begin
                                csr_old = mcause;
                                case (csr_wsc_mode_in)
                                    2'b01: csr_wdata = csr_src;
                                    2'b10: csr_wdata = csr_old | csr_src;
                                    2'b11: csr_wdata = csr_old & ~csr_src;
                                    default: csr_wdata = csr_old;
                                endcase
                                mcause <= csr_wdata;
                            end

                            default: begin
                            end
                        endcase
                    end
                end

                // ----------------------------
                // write mepc in second step
                // ----------------------------
                STATE_MEPC: begin
                    mepc <= epc_reg;
                end

                // ----------------------------
                // write mcause in third step
                // ----------------------------
                STATE_MCAUSE: begin
                    mcause <= cause_reg;
                end

                default: begin
                end
            endcase
        end
    end

endmodule
