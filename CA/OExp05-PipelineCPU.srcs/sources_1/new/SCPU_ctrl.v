`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2025/12/16 15:26:58
// Design Name: 
// Module Name: SCPU_ctrl
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////



module SCPU_ctrl( 
    input  [4:0] OPcode,      // inst[6:2]
    input  [2:0] Fun3,        // inst[14:12]
    input        Fun7,        // inst[30]
    input        MIO_ready,

    output reg [1:0] ImmSel, 
    output reg       ALUSrc_B,
    output reg [1:0] MemtoReg,
    output reg       Jump,
    output reg       Branch,
    output reg       BranchN,
    output reg       RegWrite,
    output reg       MemRW,
    output reg [2:0] ALU_Control,
    output reg       CPU_MIO
);

    reg [1:0] ALUop;
    wire [3:0] Fun = {Fun3, Fun7};

    /***********************
     * 主控制器
     ***********************/
    always @(*) begin
        // 默认值
        ALUSrc_B = 0;
        MemtoReg = 2'b00;
        RegWrite = 0;
        Branch   = 0;
        Jump     = 0;
        MemRW    = 0;
        CPU_MIO  = 0;
        ImmSel   = 2'b00;
        ALUop    = 2'b10; 

        case(OPcode)

            /*********************************
             * R-type : 01100
             *********************************/
            5'b01100: begin
                ALUSrc_B = 0;
                MemtoReg = 2'b00;
                RegWrite = 1;
                ALUop    = 2'b10;
            end

            /*********************************
             * Load : 00000  (lw)
             *********************************/
            5'b00000: begin
                ALUSrc_B = 1;
                MemtoReg = 2'b01;   // 写回 Memory 数据
                RegWrite = 1;
                MemRW    = 0;
                ImmSel   = 2'b00;   // I-type
                ALUop    = 2'b00;   // add 计算地址
            end

            /*********************************
             * Store : 01000 (sw)
             *********************************/
            5'b01000: begin
                ALUSrc_B = 1;
                RegWrite = 0;
                MemRW    = 1;
                ImmSel   = 2'b01;   // S-type
                ALUop    = 2'b00;   // add 地址
            end

            /*********************************
             * Branch : beq 11000
             ********************************
            5'b11000: begin
                ALUSrc_B = 0;
                Branch   = 1;
                ImmSel   = 2'b10;   // B-type
                ALUop    = 2'b01;   // sub 比较
            end*/
            
            
            // -------------------------------------------------
            // B-type (beq / bne)
            // -------------------------------------------------
            5'b11000: begin
                ALUSrc_B = 1'b0;
                MemtoReg = 2'b00;
                ImmSel   = 2'b10;
                RegWrite = 1'b0;
                MemRW    = 1'b0;
                Jump = 2'b00;
                ALUop    = 2'b01;        // sub

                if (Fun3 == 3'b000) begin
                    Branch  = 1'b1;      // beq
                    BranchN = 1'b0;
                end
                else if (Fun3 == 3'b001) begin
                    Branch  = 1'b0;
                    BranchN = 1'b1;      // bne
                end
            end
            
            /*********************************
             * Jump : jal 11011
             *********************************/
            5'b11011: begin
                Jump     = 1;
                RegWrite = 1;
                MemtoReg = 2'b10;   // PC + 4
                ImmSel   = 2'b11;   // J-type
                ALUSrc_B = 1'b1;
                ALUop    = 2'b00;   // add
            end

            /*********************************
             * I-type 算术 : addi/xori/andi/ori... 00100
             *********************************/
            5'b00100: begin
                ALUSrc_B = 1;
                RegWrite = 1;
                MemtoReg = 2'b00;   // 写回 ALU 结果（修复严重错误）
                ImmSel   = 2'b00;   // I-type
                ALUop    = 2'b11;   // I-type ALU
            end

            default: begin
                // 全保持默认值
            end
        endcase
    end

    always @(*) begin
        case(ALUop)

            // Load/Store: add
            2'b00: ALU_Control = 3'b010;

            // Branch: sub
            2'b01: ALU_Control = 3'b110;

            // R-type
            2'b10: begin
                case(Fun)
                    4'b0000: ALU_Control = 3'b010; // add
                    4'b0001: ALU_Control = 3'b110; // sub
                    4'b1110: ALU_Control = 3'b000; // and
                    4'b1100: ALU_Control = 3'b001; // or
                    4'b1000: ALU_Control = 3'b011; // xor
                    4'b0100: ALU_Control = 3'b111; // slt
                    4'b1010: ALU_Control = 3'b101; // srl
                    default: ALU_Control = 3'b000;
                endcase
            end

            // I-type 算术（与 R-type 相同，但只依赖 Fun3）
            2'b11: begin
                case(Fun3)
                    3'b000: ALU_Control = 3'b010; // addi
                    3'b111: ALU_Control = 3'b000; // andi
                    3'b110: ALU_Control = 3'b001; // ori
                    3'b100: ALU_Control = 3'b011; // xori
                    3'b010: ALU_Control = 3'b111; // slti
                    3'b101: ALU_Control = 3'b101; // srli
                    default: ALU_Control = 3'b000;
                endcase
            end

            default: ALU_Control = 3'b000;
        endcase
    end
endmodule

