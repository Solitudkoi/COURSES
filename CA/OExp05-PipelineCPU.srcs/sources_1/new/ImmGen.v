`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2025/12/16 15:27:14
// Design Name: 
// Module Name: ImmGen
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


module ImmGen(
 input [1:0] ImmSel,
 input [31:0] inst_field,
 output reg [31:0] Imm_out
);
 always @(*) begin
 case (ImmSel)
 // I-type
 2'b00: Imm_out = {{20{inst_field[31]}}, inst_field[31:20]};
 // S-type
 2'b01: Imm_out = {{20{inst_field[31]}}, inst_field[31:25],
inst_field[11:7]};
 // B-type
 2'b10: Imm_out = {{20{inst_field[31]}}, inst_field[7],
inst_field[30:25], inst_field[11:8], 1'b0};
 // J-type
// 2'b11: Imm_out = {inst_field[31:12], 12'b0};
// J-type
2'b11: Imm_out = {
    {11{inst_field[31]}},
    inst_field[31],
    inst_field[19:12],
    inst_field[20],
    inst_field[30:21],
    1'b0
};

 endcase
 end
endmodule


