`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2025/09/25 20:58:41
// Design Name: 
// Module Name: ALU
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



module ALU(
    input  [31:0] A, B,
    input  [2:0]  ALU_operation,
    output [31:0] res,
    output        zero
);

    wire [31:0] and_res, or_res, xor_res, nor_res;
    wire [31:0] add_res, sub_res, srl_res, slt_res;
    wire Co_add, Co_sub;

    and32 u_and(.A(A), .B(B), .res(and_res));
    or32  u_or (.A(A), .B(B), .res(or_res));
    xor32 u_xor(.A(A), .B(B), .res(xor_res));
    nor32 u_nor(.A(A), .B(B), .res(nor_res));
    srl32 u_srl(.A(A), .B(B), .res(srl_res));
    ADC32 u_add(.A(A), .B(B), .C0(1'b0), .S(add_res), .Co(Co_add));
    ADC32 u_sub(.A(A), .B(~B), .C0(1'b1), .S(sub_res), .Co(Co_sub));
    
    wire setLess = sub_res[31];
    //assign slt_res = {31'b0, setLess};
   assign slt_res = ($signed(A) < $signed(B)) ? 32'h00000001 : 32'h00000000;

    mux8to1_32 u_mux(
        .d0(and_res),  
        .d1(or_res),   
        .d2(add_res),  
        .d3(xor_res),   
        .d4(nor_res),  
        .d5(srl_res),  
        .d6(sub_res),  
        .d7(slt_res),  
        .y(res),
        .sel(ALU_operation)
    );

    wire or_out;
    or_bit_32 u_orbit(.A(res), .Y(or_out));
    assign zero = ~or_out;
endmodule
