`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2025/12/16 14:56:14
// Design Name: 
// Module Name: Pipeline_CPU
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

`timescale 1ns / 1ps
module Pipeline_CPU(
 input clk, // 时钟
 input rst, // 复位
 input[31:0] Data_in, // 存储器数据输入
 input[31:0] inst_IF, // 取指阶段指令
 output [31:0] PC_out_IF, // 取指阶段 PC 输出
 output [31:0] PC_out_ID, // 译码阶段 PC 输出
 output [31:0] inst_ID, // 译码阶段指令
 output [31:0] PC_out_EX, // 执行阶段 PC 输出
 output [31:0] MemRW_EX, // 执行阶段存储器读写
 output [31:0] MemRW_Mem, // 访存阶段存储器读写
 output [31:0] Addr_out, // 地址输出
 output [31:0] Data_out, // CPU 数据输出
 output [31:0] Data_out_WB // 写回数据输出
);
wire Pipeline_Mem_PCSrc;
wire [31:0] Pipeline_IF_PC_out_IF;
wire [31:0] IF_reg_ID_PC_out_IFID;
wire [31:0] IF_reg_ID_inst_out_IFID;
wire [31:0] Pipeline_WB_Data_out_WB;
wire [31:0] Pipeline_ID_Rd_addr_out_ID;
wire [31:0] Pipeline_ID_Rs1_out_ID;
wire [31:0] Pipeline_ID_Rs2_out_ID;
wire [31:0] Pipeline_ID_Imm_out_ID;
wire Pipeline_ID_ALUSrc_B_ID;
wire [2:0] Pipeline_ID_ALU_control_ID;
wire Pipeline_ID_Branch_ID;
wire Pipeline_ID_BranchN_ID;
wire Pipeline_ID_MemRW_ID;
wire Pipeline_ID_Jump_ID;
wire [1:0] Pipeline_ID_MemtoReg_ID;
wire Pipeline_ID_RegWrite_out_ID;
wire [31:0] ID_reg_Ex_PC_out_IDEX;
wire [4:0] ID_reg_Ex_Rd_addr_out_IDEX;
wire [31:0] ID_reg_Ex_Rs1_out_IDEX;
wire [31:0] ID_reg_Ex_Rs2_out_IDEX;
wire [31:0] ID_reg_Ex_Imm_out_IDEX;
wire ID_reg_Ex_ALUSrc_B_out_IDEX;
wire [2:0] ID_reg_Ex_ALU_control_out_IDEX;
wire ID_reg_Ex_Branch_out_IDEX;
wire ID_reg_Ex_BranchN_out_IDEX;
wire ID_reg_Ex_MemRW_out_IDEX;
wire ID_reg_Ex_Jump_out_IDEX;
wire [1:0] ID_reg_Ex_MemtoReg_out_IDEX;
wire ID_reg_Ex_RegWrite_out_IDEX;
wire [31:0] Pipeline_Ex_PC_out_EX;
wire [31:0] Pipeline_Ex_PC4_out_EX;
wire Pipeline_Ex_zero_out_EX;
wire [31:0] Pipeline_Ex_ALU_out_EX;
wire [31:0] Pipeline_Ex_Rs2_out_EX;
wire [31:0] Ex_reg_Mem_PC_out_EXMem;
wire [31:0] Ex_reg_Mem_PC4_out_EXMem;
wire [4:0] Ex_reg_Mem_Rd_addr_out_EXMem;
wire Ex_reg_Mem_zero_out_EXMem;
wire [31:0] Ex_reg_Mem_ALU_out_EXMem;
wire [31:0] Ex_reg_Mem_Rs2_out_EXMem;
wire Ex_reg_Mem_Branch_out_EXMem;
wire Ex_reg_Mem_BranchN_out_EXMem;
wire Ex_reg_Mem_MemRW_out_EXMem;
wire Ex_reg_Mem_Jump_out_EXMem;
wire [1:0] Ex_reg_Mem_MemtoReg_out_EXMem;
wire Ex_reg_Mem_RegWrite_out_EXMem;
wire [31:0] Mem_reg_WB_PC4_out_MemWB;
wire [4:0] Mem_reg_WB_Rd_addr_out_MemWB;
wire [31:0] Mem_reg_WB_ALU_out_MemWB;
wire [31:0] Mem_reg_WB_DMem_data_out_MemWB;
wire [1:0] Mem_reg_WB_MemtoReg_out_MemWB;
wire Mem_reg_WB_RegWrite_out_MemWB;
Pipeline_IF Pipeline_CPU_IF(
 .clk_IF(clk),
 .rst_IF(rst),
 .en_IF(1'b1),
 .PC_in_IF(Ex_reg_Mem_PC_out_EXMem),
 .PCSrc(Pipeline_Mem_PCSrc),
 .PC_out_IF(Pipeline_IF_PC_out_IF)
);
IF_reg_ID Pipeline_IF_reg_ID(
 .clk_IFID(clk),
 .rst_IFID(rst),
 .en_IFID(1'b1),
 .PC_in_IFID(Pipeline_IF_PC_out_IF),
 .inst_in_IFID(inst_IF),
 .PC_out_IFID(IF_reg_ID_PC_out_IFID),
 .inst_out_IFID(IF_reg_ID_inst_out_IFID)
);
Pipeline_ID Pipeline_CPU_ID(
 .clk_ID(clk),
 .rst_ID(rst),
 .RegWrite_in_ID(Mem_reg_WB_RegWrite_out_MemWB),
 .Rd_addr_ID(Mem_reg_WB_Rd_addr_out_MemWB),
 .Wt_data_ID(Pipeline_WB_Data_out_WB),
 .Inst_in_ID(IF_reg_ID_inst_out_IFID),
 .Rd_addr_out_ID(Pipeline_ID_Rd_addr_out_ID),
 .Rs1_out_ID(Pipeline_ID_Rs1_out_ID),
 .Rs2_out_ID(Pipeline_ID_Rs2_out_ID),
 .Imm_out_ID(Pipeline_ID_Imm_out_ID),
 .ALUSrc_B_ID(Pipeline_ID_ALUSrc_B_ID),
 .ALU_control_ID(Pipeline_ID_ALU_control_ID),
 .Branch_ID(Pipeline_ID_Branch_ID),
 .BranchN_ID(Pipeline_ID_BranchN_ID),
 .MemRW_ID(Pipeline_ID_MemRW_ID),
 .Jump_ID(Pipeline_ID_Jump_ID),
 .MemtoReg_ID(Pipeline_ID_MemtoReg_ID),
 .RegWrite_out_ID(Pipeline_ID_RegWrite_out_ID)
);
ID_reg_Ex Pipeline_ID_reg_Ex(
 .clk_IDEX(clk),
 .rst_IDEX(rst),
 .en_IDEX(1'b1),
 .PC_in_IDEX(IF_reg_ID_PC_out_IFID),
 .Rd_addr_IDEX(Pipeline_ID_Rd_addr_out_ID),
 .Rs1_in_IDEX(Pipeline_ID_Rs1_out_ID),
 .Rs2_in_IDEX(Pipeline_ID_Rs2_out_ID),
 .Imm_in_IDEX(Pipeline_ID_Imm_out_ID),
 .ALUSrc_B_in_IDEX(Pipeline_ID_ALUSrc_B_ID),
 .ALU_control_in_IDEX(Pipeline_ID_ALU_control_ID),
 .Branch_in_IDEX(Pipeline_ID_Branch_ID),
 .BranchN_in_IDEX(Pipeline_ID_BranchN_ID),
 .MemRW_in_IDEX(Pipeline_ID_MemRW_ID),
 .Jump_in_IDEX(Pipeline_ID_Jump_ID),
 .MemtoReg_in_IDEX(Pipeline_ID_MemtoReg_ID),
 .RegWrite_in_IDEX(Pipeline_ID_RegWrite_out_ID),
 .PC_out_IDEX(ID_reg_Ex_PC_out_IDEX),
 .Rd_addr_out_IDEX(ID_reg_Ex_Rd_addr_out_IDEX),
 .Rs1_out_IDEX(ID_reg_Ex_Rs1_out_IDEX),
 .Rs2_out_IDEX(ID_reg_Ex_Rs2_out_IDEX),
 .Imm_out_IDEX(ID_reg_Ex_Imm_out_IDEX),
 .ALUSrc_B_out_IDEX(ID_reg_Ex_ALUSrc_B_out_IDEX),
 .ALU_control_out_IDEX(ID_reg_Ex_ALU_control_out_IDEX),
 .Branch_out_IDEX(ID_reg_Ex_Branch_out_IDEX),
 .BranchN_out_IDEX(ID_reg_Ex_BranchN_out_IDEX),
 .MemRW_out_IDEX(ID_reg_Ex_MemRW_out_IDEX),
 .Jump_out_IDEX(ID_reg_Ex_Jump_out_IDEX),
 .MemtoReg_out_IDEX(ID_reg_Ex_MemtoReg_out_IDEX),
 .RegWrite_out_IDEX(ID_reg_Ex_RegWrite_out_IDEX)
);
Pipeline_Ex Pipeline_CPU_Ex(
 .PC_in_EX(ID_reg_Ex_PC_out_IDEX),
 .Rs1_in_EX(ID_reg_Ex_Rs1_out_IDEX),
 .Rs2_in_EX(ID_reg_Ex_Rs2_out_IDEX),
 .Imm_in_EX(ID_reg_Ex_Imm_out_IDEX),
 .ALUSrc_B_in_EX(ID_reg_Ex_ALUSrc_B_out_IDEX),
 .ALU_control_in_EX(ID_reg_Ex_ALU_control_out_IDEX),
 .PC_out_EX(Pipeline_Ex_PC_out_EX),
 .PC4_out_EX(Pipeline_Ex_PC4_out_EX),
 .zero_out_EX(Pipeline_Ex_zero_out_EX),
 .ALU_out_EX(Pipeline_Ex_ALU_out_EX),
 .Rs2_out_EX(Pipeline_Ex_Rs2_out_EX)
);
Ex_reg_Mem Pipeline_Ex_reg_Mem(
 .clk_EXMem(clk),
 .rst_EXMem(rst),
 .en_EXMem(1'b1),
 .PC_in_EXMem(Pipeline_Ex_PC_out_EX),
 .PC4_in_EXMem(Pipeline_Ex_PC4_out_EX),
 .Rd_addr_EXMem(ID_reg_Ex_Rd_addr_out_IDEX),
 .zero_in_EXMem(Pipeline_Ex_zero_out_EX),
 .ALU_in_EXMem(Pipeline_Ex_ALU_out_EX),
 .Rs2_in_EXMem(Pipeline_Ex_Rs2_out_EX),
 .Branch_in_EXMem(ID_reg_Ex_Branch_out_IDEX),
 .BranchN_in_EXMem(ID_reg_Ex_BranchN_out_IDEX),
 .MemRW_in_EXMem(ID_reg_Ex_MemRW_out_IDEX),
 .Jump_in_EXMem(ID_reg_Ex_Jump_out_IDEX),
 .MemtoReg_in_EXMem(ID_reg_Ex_MemtoReg_out_IDEX),
 .RegWrite_in_EXMem(ID_reg_Ex_RegWrite_out_IDEX),
 .PC_out_EXMem(Ex_reg_Mem_PC_out_EXMem),
 .PC4_out_EXMem(Ex_reg_Mem_PC4_out_EXMem),
 .Rd_addr_out_EXMem(Ex_reg_Mem_Rd_addr_out_EXMem),
 .zero_out_EXMem(Ex_reg_Mem_zero_out_EXMem),
 .ALU_out_EXMem(Ex_reg_Mem_ALU_out_EXMem),
 .Rs2_out_EXMem(Ex_reg_Mem_Rs2_out_EXMem),
 .Branch_out_EXMem(Ex_reg_Mem_Branch_out_EXMem),
 .BranchN_out_EXMem(Ex_reg_Mem_BranchN_out_EXMem),
 .MemRW_out_EXMem(Ex_reg_Mem_MemRW_out_EXMem),
 .Jump_out_EXMem(Ex_reg_Mem_Jump_out_EXMem),
 .MemtoReg_out_EXMem(Ex_reg_Mem_MemtoReg_out_EXMem),
 .RegWrite_out_EXMem(Ex_reg_Mem_RegWrite_out_EXMem)
);
Pipeline_Mem Pipeline_CPU_Mem(
 .zero_in_Mem(Ex_reg_Mem_zero_out_EXMem),
 .Branch_in_Mem(Ex_reg_Mem_Branch_out_EXMem),
 .BranchN_in_Mem(Ex_reg_Mem_BranchN_out_EXMem),
 .Jump_in_Mem(Ex_reg_Mem_Jump_out_EXMem),
 .PCSrc(Pipeline_Mem_PCSrc)
);
Mem_reg_WB Pipeline_Mem_reg_WB(
 .clk_MemWB(clk),
 .rst_MemWB(rst),
 .en_MemWB(1'b1),
 .PC4_in_MemWB(Ex_reg_Mem_PC4_out_EXMem),
 .Rd_addr_MemWB(Ex_reg_Mem_Rd_addr_out_EXMem),
 .ALU_in_MemWB(Ex_reg_Mem_ALU_out_EXMem),
 .DMem_data_MemWB(Data_in),
 .MemtoReg_in_MemWB(Ex_reg_Mem_MemtoReg_out_EXMem),
 .RegWrite_in_MemWB(Ex_reg_Mem_RegWrite_out_EXMem),
 .PC4_out_MemWB(Mem_reg_WB_PC4_out_MemWB),
 .Rd_addr_out_MemWB(Mem_reg_WB_Rd_addr_out_MemWB),
 .ALU_out_MemWB(Mem_reg_WB_ALU_out_MemWB),
 .DMem_data_out_MemWB(Mem_reg_WB_DMem_data_out_MemWB),
 .MemtoReg_out_MemWB(Mem_reg_WB_MemtoReg_out_MemWB),
 .RegWrite_out_MemWB(Mem_reg_WB_RegWrite_out_MemWB)
);
Pipeline_WB Pipeline_CPU_WB(
 .PC4_in_WB(Mem_reg_WB_PC4_out_MemWB),
 .ALU_in_WB(Mem_reg_WB_ALU_out_MemWB),
 .DMem_data_WB(Mem_reg_WB_DMem_data_out_MemWB),
 .MemtoReg_in_WB(Mem_reg_WB_MemtoReg_out_MemWB),
 .Data_out_WB(Pipeline_WB_Data_out_WB)
);
assign PC_out_EX = Pipeline_Ex_PC_out_EX;
assign PC_out_ID = IF_reg_ID_PC_out_IFID;
assign inst_ID = IF_reg_ID_inst_out_IFID;
assign PC_out_IF = Pipeline_IF_PC_out_IF;
assign Addr_out = Ex_reg_Mem_ALU_out_EXMem;
assign Data_out = Ex_reg_Mem_Rs2_out_EXMem;
assign Data_out_WB = Pipeline_WB_Data_out_WB;
assign MemRW_Mem = Ex_reg_Mem_MemRW_out_EXMem;
assign MemRW_EX = ID_reg_Ex_MemRW_out_IDEX;
endmodule