`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2025/12/16 15:49:17
// Design Name: 
// Module Name: soc_test_wrapper
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
module soc_test_wrapper(
 input clk,
 input rst
);
 wire [31:0] RAM_B_douta;
 wire [31:0] CPU_inst_in;
 wire [31:0] CPU_Addr_out;
 wire CPU_MemRW;
 wire [31:0] CPU_Data_out;
 wire [31:0] CPU_PC_out;
 wire [9:0] Addr_out_slice;
 wire [9:0] PC_out_slice;
 wire [31:0] CPU_PC_out_ID;
 wire [31:0] CPU_PC_out_Ex;
 wire [31:0] CPU_inst_ID;
 wire CPU_MemRW_Ex;
 RAM_B RAM_B_0(
 .clka(~clk),
 .wea(CPU_MemRW),
 .addra(Addr_out_slice),
 .dina(CPU_Data_out),
 .douta(RAM_B_douta)
 );
 dist_mem_gen_0 ROM_0(
 .a(PC_out_slice),
 .spo(CPU_inst_in)
 );
 
 Pipeline_CPU Pipeline_CPU_wrapper_0(
 .clk(clk),
 .rst(rst),
 .Data_in(RAM_B_douta),
 .inst_IF(CPU_inst_in),
 .PC_out_IF(CPU_PC_out),
 .PC_out_ID(CPU_PC_out_ID),
 .inst_ID(CPU_inst_ID),
 .PC_out_EX(CPU_PC_out_Ex),
 .MemRW_EX(CPU_MemRW_Ex),
 .MemRW_Mem(CPU_MemRW),
 .Addr_out(CPU_Addr_out),
 .Data_out(CPU_Data_out)
 );
 
 assign Addr_out_slice = CPU_Addr_out[11:2];
 assign PC_out_slice = CPU_PC_out[11:2];
endmodule