
// System Reset Signal Gen

module sys_reset (
	RSTn, CLK, DOUT
);

input RSTn;
input CLK;
output DOUT;

reg [15:0]count;
wire count_up;

	always @(negedge RSTn or posedge CLK) begin
		if(~RSTn)
			count <= 16'h0000;
		else if(count_up==1'b0)
			count <= count + 1;
	end

//	count_up <= '1' when count=X"FFFF" else '0';
	assign count_up = count[15];

	assign DOUT = ~count_up;

endmodule
