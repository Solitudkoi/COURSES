`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2025/11/27 09:04:00
// Design Name: 
// Module Name: Ext_32
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


module Ext_32(
    input  wire [15:0] imm_16,
    output wire [31:0] Imm_32
);
    assign Imm_32 = {{16{imm_16[15]}}, imm_16};
endmodule

