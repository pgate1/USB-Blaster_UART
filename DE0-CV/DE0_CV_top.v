
module DE0_CV_top
(
	input wire RESET_N,
	////////////////////	Clock Input	 	////////////////////
	input wire CLOCK_50,  //	50 MHz
	input wire CLOCK2_50, //	50 MHz
	input wire CLOCK3_50, //	50 MHz
	input wire CLOCK4_50, //	50 MHz
	////////////////////	Push Button		////////////////////
	input wire [3:0] KEY, //	Pushbutton[3:0]
	////////////////////	DPDT Switch		////////////////////
	input wire [9:0] SW, //	Toggle Switch[9:0]
	////////////////////	7-SEG Dispaly	////////////////////
	output wire [6:0] HEX0, //	Seven Segment Digit 0
	output wire [6:0] HEX1, //	Seven Segment Digit 1
	output wire [6:0] HEX2,	//	Seven Segment Digit 2
	output wire [6:0] HEX3, //	Seven Segment Digit 3
	output wire [6:0] HEX4, //	Seven Segment Digit 4
	output wire [6:0] HEX5, //	Seven Segment Digit 5
	////////////////////////	LED		////////////////////////
	output wire [9:0] LEDR, //	LED Green[9:0]
	/////////////////////	SDRAM Interface		////////////////
	inout wire [15:0] DRAM_DQ, //	SDRAM Data bus 16 Bits
	output wire [12:0] DRAM_ADDR, //	SDRAM Address bus 13 Bits
	output wire DRAM_LDQM, //	SDRAM Low-byte Data Mask 
	output wire DRAM_UDQM, //	SDRAM High-byte Data Mask
	output wire DRAM_WE_N, //	SDRAM Write Enable
	output wire DRAM_CAS_N, //	SDRAM Column Address Strobe
	output wire DRAM_RAS_N, //	SDRAM Row Address Strobe
	output wire DRAM_CS_N, //	SDRAM Chip Select
	output wire [1:0] DRAM_BA, //	SDRAM Bank Address
	output wire DRAM_CLK, //	SDRAM Clock
	output wire DRAM_CKE, //	SDRAM Clock Enable
	////////////////////	SD_Card Interface	////////////////
	input wire [3:0] SD_DATA, //	SD Card Data
	inout wire SD_CMD, //	SD Card Command Signal
	output wire SD_CLK, //	SD Card Clock
	////////////////////	PS2		////////////////////////////
	input wire PS2_CLK, //	PS2 
	input wire PS2_DAT, //	PS2 
	input wire PS2_CLK2, //	PS2
	input wire PS2_DAT2, //	PS2
	////////////////////	VGA		////////////////////////////
	output wire VGA_HS, //	VGA H_SYNC
	output wire VGA_VS, //	VGA V_SYNC
	output wire [3:0] VGA_R, //	VGA Red[3:0]
	output wire [3:0] VGA_G, //	VGA Green[3:0]
	output wire [3:0] VGA_B, //	VGA Blue[3:0]
	////////////////////	GPIO	////////////////////////////
	inout wire [35:0] GPIO_0, //	GPIO Connection 0 Data Bus
	inout wire [35:0] GPIO_1  //	GPIO Connection 1 Data Bus
);

wire reset, g_reset;

wire [15:0] sdram_Dout;
wire sdram_Dout_En;

wire sd_cmd_out, sd_cmd_en;

	sys_reset RSTU (
		.RSTn(RESET_N), .CLK(CLOCK_50), .DOUT(reset)
	);

	GLOBAL rst_GU (
		.IN(reset), .OUT(g_reset)
	);

	vjtag_test CU (
		.p_reset(g_reset),
		.m_clock(CLOCK_50),
		.KEY(KEY), // in std_logic_vector(3 downto 0);
		.SW(SW), // in std_logic_vector(9 downto 0);
		.HEX0(HEX0), //	Seven Segment Digit 0
		.HEX1(HEX1), //	Seven Segment Digit 1
		.HEX2(HEX2), //	Seven Segment Digit 2
		.HEX3(HEX3), //	Seven Segment Digit 3
		.HEX4(HEX4), //	Seven Segment Digit 4
		.HEX5(HEX5), //	Seven Segment Digit 5
		.LEDR(LEDR), // out std_logic_vector(9 downto 0))
//--------------------- SDRAM Interface --------------------
		.SDRAM_CSn(DRAM_CS_N), .SDRAM_WEn(DRAM_WE_N), .SDRAM_DEn(sdram_Dout_En),
		.SDRAM_RASn(DRAM_RAS_N), .SDRAM_CASn(DRAM_CAS_N),
		.SDRAM_BA(DRAM_BA), .SDRAM_ADDR(DRAM_ADDR),
		.SDRAM_LDQM(DRAM_LDQM), .SDRAM_UDQM(DRAM_UDQM),
		.SDRAM_Din(DRAM_DQ), .SDRAM_Dout(sdram_Dout)
	);

	assign DRAM_CKE = 1'b1;
	sdram_pll sdram_pll_inst (
		.refclk(CLOCK_50),
	//	rst => not RESET_N,
		.outclk_0(DRAM_CLK)
	);
	assign DRAM_DQ = sdram_Dout_En==1'b0 ? sdram_Dout : 16'hzzzz;

	assign GPIO_0 = 36'b0;
	assign GPIO_1 = 36'b0;

endmodule
