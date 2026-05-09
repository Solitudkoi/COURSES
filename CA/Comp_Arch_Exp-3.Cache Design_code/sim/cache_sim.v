`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   13:26:56 01/15/2021
// Design Name:   cache
// Module Name:   cache_sim.v
// Project Name:  Topic8
// Target Device:  
// Tool versions:  
// Description:   Cache testbench with key signal monitoring
//
////////////////////////////////////////////////////////////////////////////////

module cache_sim;

	// Inputs
	reg clk;
	reg rst;
	reg [31:0] addr;
	reg load;
	reg store;
	reg edit;
	reg invalid;
	reg [2:0] u_b_h_w;
	reg [31:0] din;

	// Outputs
	wire hit;
	wire [31:0] dout;
	wire valid;
	wire dirty;
	wire [22:0] tag;

	// Instantiate the Unit Under Test (UUT)
	cache uut (
		.clk(~clk), 
		.rst(rst), 
		.addr(addr), 
		.load(load),
		.store(store), 
		.edit(edit), 
		.invalid(invalid), 
		.u_b_h_w(u_b_h_w),
		.din(din), 
		.hit(hit), 
		.dout(dout), 
		.valid(valid), 
		.dirty(dirty), 
		.tag(tag)
	);

	initial begin
		clk = 1;
		forever #10 clk = ~clk ;
	end

	reg [31:0] counter = 0;

	// ----------------------------------------------------------------
	// Delayed registers: capture command signals at posedge so we can
	// print the cache response (which is registered one cycle later)
	// ----------------------------------------------------------------
	reg        d_load, d_store, d_edit;
	reg [31:0] d_addr;
	reg [31:0] d_din;
	reg [2:0]  d_u_b_h_w;

	always @(posedge clk) begin
		d_load    <= load;
		d_store   <= store;
		d_edit    <= edit;
		d_addr    <= addr;
		d_din     <= din;
		d_u_b_h_w <= u_b_h_w;
	end

	// ----------------------------------------------------------------
	// Print header once at start
	// ----------------------------------------------------------------
	initial begin
		$display("==========================================================");
		$display(" Cache Simulation Key Signal Monitor");
		$display("==========================================================");
		$display("[Cyc] CMD   ADDR       DIN        | HIT VALID DIRTY TAG        DOUT");
		$display("----------------------------------------------------------");
	end

	// ----------------------------------------------------------------
	// Print key signals one cycle after each command
	// (cache outputs are registered, so valid one cycle after input)
	// ----------------------------------------------------------------
	always @(negedge clk) begin
		// Only print when a command was active last cycle
		if (d_load | d_store | d_edit) begin
			$write("[%3d] ", counter);

			if (d_load)       $write("LOAD  ");
			else if (d_store) $write("STORE ");
			else if (d_edit)  $write("EDIT  ");
			else              $write("IDLE  ");

			$write("0x%08h  0x%08h | ", d_addr, d_din);
			$write("%b    %b     %b     0x%06h  0x%08h",
			        hit, valid, dirty, tag, dout);

			// Annotate hit / miss
			if (d_load | d_edit) begin
				if (hit) $write("  << HIT");
				else     $write("  << MISS");
			end else if (d_store) begin
				// store always replaces (miss scenario)
				$write("  [REPLACE: evict valid=%b dirty=%b tag=0x%06h]",
				        valid, dirty, tag);
			end

			$display("");
		end
	end

	// ----------------------------------------------------------------
	// Stimulus
	// ----------------------------------------------------------------
	always @(posedge clk) begin
		counter <= counter + 32'b1;

		case (counter)
			// Initialize Inputs
			32'd0: begin
				rst       <= 0;
				addr      <= 0;
				load      <= 0;
				store     <= 0;
				edit      <= 0;
				invalid   <= 0;
				u_b_h_w   <= 0;
				din       <= 0;
			end

			// ---- init: store 4 words into set 0 (way0 and way1) ----
			32'd10: begin
				$display("--- [Cycle %0d] BEGIN INIT: store 4 words to set 0 ---", counter);
				load      <= 0;
				store     <= 1;
				edit      <= 0;
				u_b_h_w   <= 3'b010;  // word
				din       <= 32'h11111111;
				addr      <= 32'h00000004;  // set=0, word=1
			end

			32'd11: begin
				addr      <= 32'h0000000C;  // set=0, word=3
			end

			32'd12: begin
				addr      <= 32'h00000010;  // set=1, word=0  (index=1)
			end

			32'd13: begin
				addr      <= 32'h00000014;  // set=1, word=1
			end

			// ---- read miss ----
			32'd14: begin
				$display("--- [Cycle %0d] READ MISS: addr=0x00000020 (set=2, not stored) ---", counter);
				load      <= 1;
				store     <= 0;
				edit      <= 0;
				u_b_h_w   <= 3'b010;
				din       <= 0;
				addr      <= 32'h00000020;  // set=2, not loaded yet => miss
			end

			// ---- read hit ----
			32'd15: begin
				$display("--- [Cycle %0d] READ HIT:  addr=0x00000010 (set=1, stored at cycle 12) ---", counter);
				u_b_h_w   <= 3'b010;
				addr      <= 32'h00000010;
			end

			// ---- write miss ----
			32'd16: begin
				$display("--- [Cycle %0d] WRITE MISS: addr=0x00000024 (set=2, not in cache) ---", counter);
				load      <= 0;
				store     <= 0;
				edit      <= 1;
				u_b_h_w   <= 3'b010;
				din       <= 32'h22222222;
				addr      <= 32'h00000024;  // set=2, not in cache => miss
			end

			// ---- write hit ----
			32'd17: begin
				$display("--- [Cycle %0d] WRITE HIT:  addr=0x00000014 (set=1, stored at cycle 13) ---", counter);
				u_b_h_w   <= 3'b010;
				addr      <= 32'h00000014;
			end

			// ---- read set 0 way0 to set its recent bit ----
			32'd18: begin
				$display("--- [Cycle %0d] LOAD set0/way0: addr=0x00000004 => set recent[0]=1 ---", counter);
				load      <= 1;
				store     <= 0;
				edit      <= 0;
				u_b_h_w   <= 3'b010;
				din       <= 0;
				addr      <= 32'h00000004;
			end

			// ---- store to set 0, should evict way1 (recent=way0) ----
			32'd19: begin
				$display("--- [Cycle %0d] STORE to set0 (new tag): way0 recent => evict way1 ---", counter);
				load      <= 0;
				store     <= 1;
				edit      <= 0;
				u_b_h_w   <= 3'b010;
				din       <= 32'h33333333;
				addr      <= 32'h00000204;  // different tag, same set 0
			end

			// ---- edit set 0 way1 -> dirty+recent ----
			32'd20: begin
				$display("--- [Cycle %0d] EDIT set0 (hit): addr=0x00000204 => dirty & recent ---", counter);
				load      <= 0;
				store     <= 0;
				edit      <= 1;
				u_b_h_w   <= 3'b010;
				din       <= 32'h44444444;
				addr      <= 32'h00000204;
			end

			// ---- read set0 way0, flip recent ----
			32'd21: begin
				$display("--- [Cycle %0d] LOAD set0/way0: addr=0x00000004 => flip recent to way0 ---", counter);
				load      <= 1;
				store     <= 0;
				edit      <= 0;
				u_b_h_w   <= 3'b010;
				din       <= 0;
				addr      <= 32'h00000004;
			end

			// ---- read miss with tag mismatch; LRU=way1(dirty) must be written back ----
			32'd22: begin
				$display("--- [Cycle %0d] LOAD MISS (tag mismatch, set0): check dirty eviction ---", counter);
				load      <= 1;
				store     <= 0;
				edit      <= 0;
				u_b_h_w   <= 3'b010;
				din       <= 32'h0;
				addr      <= 32'h00000404;  // new tag for set 0 => miss, evict LRU way
			end

			// ---- replace LRU line in set 0 ----
			32'd23: begin
				$display("--- [Cycle %0d] STORE: auto-replace LRU line of set0 ---", counter);
				load      <= 0;
				store     <= 1;
				edit      <= 0;
				u_b_h_w   <= 3'b010;
				din       <= 32'h55555555;
				addr      <= 32'h00000404;
			end

			// clear
			default: begin
				load      <= 0;
				store     <= 0;
				edit      <= 0;
				din       <= 0;
				addr      <= 0;
			end
		endcase
	end

	// ----------------------------------------------------------------
	// End of simulation
	// ----------------------------------------------------------------
	initial begin
		#600;
		$display("==========================================================");
		$display(" Simulation complete.");
		$display("==========================================================");
		$finish;
	end
      
endmodule