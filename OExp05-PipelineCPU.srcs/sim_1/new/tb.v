`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2025/12/16 16:50:30
// Design Name: 
// Module Name: tb
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
/*
module tb();

    reg clk;
    reg rst;
    
    soc_test_wrapper u(
        .clk(clk),
        .rst(rst)
    );

    // clock: 100MHz
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    // reset: 至少保持几个时钟周期
    initial begin
        rst = 1;
        #40;         // 4 个时钟周期
        rst = 0;
    end
endmodule*/
`timescale 1ns / 1ps

module tb;

    reg clk;
    reg rst;

    // ======================
    // DUT
    // ======================
    soc_test_wrapper dut (
        .clk(clk),
        .rst(rst)
    );

    // ======================
    // Clock
    // ======================
    initial clk = 0;
    always #5 clk = ~clk;

    // ======================
    // Reset
    // ======================
    initial begin
        rst = 1;
        #50;
        rst = 0;
    end

endmodule
