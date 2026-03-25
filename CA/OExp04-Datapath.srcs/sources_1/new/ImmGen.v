`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2025/11/27 09:11:58
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
    always @(*)begin
        case(ImmSel)
            2'b00: Imm_out={{20{inst_field[31]}},inst_field[31:20]};
            2'b01: Imm_out={{20{inst_field[31]}},inst_field[31:25],inst_field[11:7]};
            2'b10: Imm_out={{20{inst_field[31]}},inst_field[7],inst_field[30:25],inst_field[11:8],1'b0};
            2'b00: Imm_out={inst_field[31:12],12'b0};
        endcase
    end
endmodule
