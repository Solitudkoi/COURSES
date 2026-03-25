`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2025/09/27 11:30:06
// Design Name: 
// Module Name: ADC32
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



module ADC32(
    input  [31:0] A, B,
    input         C0,      
    output [31:0] S,       
    output        Co      
);
    wire [32:0] sum_ext;
    assign sum_ext = {1'b0, A} + {1'b0, B} + C0;
    assign S  = sum_ext[31:0];
    assign Co = sum_ext[32];
endmodule
