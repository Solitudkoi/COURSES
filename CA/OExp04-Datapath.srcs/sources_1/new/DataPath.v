`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2025/11/27 09:20:24
// Design Name: 
// Module Name: DataPath
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


module DataPath(
    input clk,
    input rst,
    input [31:0] inst_field,
    input [31:0] Data_in,
    input [2:0] ALU_Control,
    input [1:0] ImmSel,
    input [1:0] MemtoReg,
    input ALUSrc_B,
    input Jump,
    input Branch,
    input RegWrite,
    
    output [31:0] PC_out,
    output [31:0] Data_out,
    output [31:0] ALU_out
    );
    
    wire [31:0] ImmGen_0_out;
    wire [31:0] add_32_0_c;
    wire [31:0] add_32_1_c;
    wire MUX2T1_32_1_s;
    wire [31:0] MUX2T1_32_0_o;
    wire [31:0] MUX2T1_32_1_o;
    wire [31:0] MUX2T1_32_2_o;
    wire [31:0] MUX4T1_32_0_o;
    wire [31:0] PC_Q;
    wire [31:0] Reg_Rs1_data;
    wire [31:0] Reg_Rs2_data;
    wire ALU_zero;
    wire [31:0] ALU_res;
    
    ImmGen ImmGen_0(
        .ImmSel(ImmSel),
        .inst_field(inst_field),
        .Imm_out(ImmGen_0_out)
        );
        
    add_32 add_32_0(
        .a(PC_Q),
        .b(32'd4),
        .c(add_32_0_c)
        );
        
    add_32 add_32_1(
        .a(PC_Q),
        .b(ImmGen_0_out),
        .c(add_32_1_c)
        );
        
    mux2to1_32 MUX2T1_32_1(
        .I0(add_32_0_c),
        .I1(add_32_1_c),
        .s(MUX2T1_32_1_s),
        .o(MUX2T1_32_1_o)
        );
    
    MUX4T1_32 MUX4T1_32_0(
        .s(MemtoReg),
        .I0(ALU_res),
        .I1(Data_in),
        .I2(add_32_0_c),
        .I3(add_32_0_c),
        .o(MUX4T1_32_0_o)
        );
        
    mux2to1_32 MUX2T1_32_2(
        .I0(MUX2T1_32_1_o),
        .I1(add_32_1_c),
        .s(Jump),
        .o(MUX2T1_32_2_o)
        );
 
    mux2to1_32 MUX2T1_32_0(
        .I0(Reg_Rs2_data),
        .I1(ImmGen_0_out),
        .s(ALUSrc_B),
        .o(MUX2T1_32_0_o)
        );
 
    regs Reg_0(
        .clk(clk),
        .rst(rst),
        .Rs1_addr(inst_field[19:15]),
        .Rs2_addr(inst_field[24:20]),
        .Wt_addr(inst_field[11:7]),
        .Wt_data(MUX4T1_32_0_o),
        .RegWrite(RegWrite),
        .Rs1_data(Reg_Rs1_data),
        .Rs2_data(Reg_Rs2_data)
        );
 
    ALU_wrapper ALU_0(
        .A(Reg_Rs1_data),
        .ALU_operation(ALU_Control),
        .B(MUX2T1_32_0_o),
        .res(ALU_res),
        .zero(ALU_zero)
        );
 
    REG32 PC(
        .clk(clk),
        .rst(rst),
        .CE(1'b1),
        .D(MUX2T1_32_2_o),
        .Q(PC_Q)
        );
 
    assign MUX2T1_32_1_s = Branch & ALU_zero;
    assign Data_out = Reg_Rs2_data;
    assign ALU_out = ALU_res;
    assign PC_out = PC_Q;
    
endmodule
