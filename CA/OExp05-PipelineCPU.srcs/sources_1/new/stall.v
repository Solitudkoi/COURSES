`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2025/12/23 16:44:40
// Design Name: 
// Module Name: stall
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


module stall(
 input rst_stall, // 复位
 input RegWrite_out_IDEX, // 执行阶段寄存器写控制
 input [4:0] Rd_addr_out_IDEX, // 执行阶段寄存器写地址
 input RegWrite_out_EXMem, // 访存阶段寄存器写控制
 input [4:0] Rd_addr_out_EXMem, // 访存阶段寄存器写地址
 input [4:0] Rs1_addr_ID, // 译码阶段寄存器读地址 1
 input [4:0] Rs2_addr_ID, // 译码阶段寄存器读地址 2
 input Rs1_used, // Rs1 被使用
 input Rs2_used, // Rs2 被使用
 input Branch_ID, // 译码阶段 beq
 input BranchN_ID, // 译码阶段 bne
 input Jump_ID, // 译码阶段 jal
 input Branch_out_IDEX, // 执行阶段 beq
 input BranchN_out_IDEX, // 执行阶段 bne
 input Jump_out_IDEX, // 执行阶段 jal
 input Branch_out_EXMem, // 访存阶段 beq
 input BranchN_out_EXMem, // 访存阶段 bne
 input Jump_out_EXMem, // 访存阶段 jal
 output en_IF, // 流水线寄存器的使能及 NOP 信号
 output en_IFID,
 output NOP_IFID,
 output NOP_IDEX
);
reg Data_stall;
reg Control_stall;
reg enIF;
reg enIFID;
reg NOPIFID;
reg NOPIDEX;
always @(*) begin
 if (RegWrite_out_EXMem && Rs1_used && Rs1_addr_ID != 0 &&Rd_addr_out_EXMem == Rs1_addr_ID)
 Data_stall = 1;
 else if (RegWrite_out_EXMem && Rs2_used && Rs2_addr_ID != 0
&&Rd_addr_out_EXMem == Rs2_addr_ID)
 Data_stall = 1;
 // EX hazard
 else if (RegWrite_out_IDEX&&Rs1_used&&Rs1_addr_ID != 0&&Rd_addr_out_IDEX == Rs1_addr_ID)
 Data_stall = 1;
 else if (RegWrite_out_IDEX&&Rs2_used&&Rs2_addr_ID != 0&&Rd_addr_out_IDEX == Rs2_addr_ID)
 Data_stall = 1;
 else
 Data_stall = 0;
 
 if (Data_stall) begin
 enIF = 0;
 enIFID = 0;
 NOPIDEX = 1;
 end else begin
 enIF = 1;
 enIFID = 1;
 NOPIDEX = 0;
 end
 // Control hazard
 // 优先级：数据冒险 > 分支冒险 > 中断
 // 当分支/跳转指令在流水线中存在时，继续冒险检测；
 // 当所有分支都完成后，再响应中断。
 if ((Branch_ID == 1 || BranchN_ID == 1||Jump_ID == 1) ||
(Branch_out_IDEX == 1 || BranchN_out_IDEX == 1 || Jump_out_IDEX ==
1) || (Branch_out_EXMem == 1 || BranchN_out_EXMem || Jump_out_EXMem
== 1))
 Control_stall = 1;
 else
 Control_stall = 0;
 
 if (Control_stall) begin
 NOPIFID = 1;
 end else begin
 NOPIFID = 0;
 end
end
assign en_IF = enIF;
assign en_IFID = enIFID;
assign NOP_IFID = NOPIFID;
assign NOP_IDEX = NOPIDEX;
endmodule