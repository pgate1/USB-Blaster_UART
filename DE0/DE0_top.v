
module DE0_top
(
	////////////////////	Clock Input	 	////////////////////	 
	input wire CLOCK_50,						//	50 MHz
	input wire CLOCK_50_2,					//	50 MHz
	////////////////////	Push Button		////////////////////
	input wire [2:0] BUTTON,	//	Pushbutton[2:0]
	////////////////////	DPDT Switch		////////////////////
	input wire [9:0] SW,		//	Toggle Switch[9:0]
	////////////////////	7-SEG Dispaly	////////////////////
	output wire [6:0] HEX0_D,	//	Seven Segment Digit 0
	output wire HEX0_DP,							//	Seven Segment Digit DP 0
	output wire [6:0] HEX1_D,	//	Seven Segment Digit 1
	output wire HEX1_DP,							//	Seven Segment Digit DP 1
	output wire [6:0] HEX2_D,	//	Seven Segment Digit 2
	output wire HEX2_DP,							//	Seven Segment Digit DP 2
	output wire [6:0] HEX3_D,	//	Seven Segment Digit 3
	output wire HEX3_DP,							//	Seven Segment Digit DP 3
	////////////////////////	LED		////////////////////////
	output wire [9:0] LEDG,		//	LED Green[9:0]
	////////////////////////	UART	////////////////////////
	output wire UART_TXD,						//	UART Transmitter
	input wire UART_RXD,						//	UART Receiver
	output wire UART_CTS,						//	UART Clear To Send
	input wire UART_RTS,						//	UART Request To Send
	/////////////////////	SDRAM Interface		////////////////
	inout wire [15:0] DRAM_DQ,	//	SDRAM Data bus 16 Bits
	output wire [12:0] DRAM_ADDR,	//	SDRAM Address bus 13 Bits
	output wire DRAM_LDQM,						//	SDRAM Low-byte Data Mask 
	output wire DRAM_UDQM,						//	SDRAM High-byte Data Mask
	output wire DRAM_WE_N,						//	SDRAM Write Enable
	output wire DRAM_CAS_N,						//	SDRAM Column Address Strobe
	output wire DRAM_RAS_N,						//	SDRAM Row Address Strobe
	output wire DRAM_CS_N,						//	SDRAM Chip Select
	output wire DRAM_BA_0,						//	SDRAM Bank Address 0
	output wire DRAM_BA_1,						//	SDRAM Bank Address 1
	output wire DRAM_CLK,						//	SDRAM Clock
	output wire DRAM_CKE,						//	SDRAM Clock Enable
	////////////////////	Flash Interface		////////////////
	inout wire [15:0] FL_DQ,	//	FLASH Data bus 16 Bits
	output wire [21:0] FL_ADDR,	//	FLASH Address bus 22 Bits
	output wire FL_WE_N,						//	FLASH Write Enable
	output wire FL_RST_N,						//	FLASH Reset
	output wire FL_OE_N,						//	FLASH Output Enable
	output wire FL_CE_N,						//	FLASH Chip Enable
	output wire FL_WP_N,						//	FLASH Hardware Write Protect
	output wire FL_BYTE_N,					//	FLASH Selects 8/16-bit mode
	input wire FL_RY,							//	FLASH Ready/Busy
	////////////////////	LCD Module 16X2		////////////////
	output wire LCD_BLON,							//	LCD Back Light ON/OFF
	output wire LCD_RW,							//	LCD Read/Write Select, 0 = Write, 1 = Read
	output wire LCD_EN,							//	LCD Enable
	output wire LCD_RS,							//	LCD Command/Data Select, 0 = Command, 1 = Data
	inout wire [7:0] LCD_DATA,	//	LCD Data bus 8 bits
	////////////////////	SD_Card Interface	////////////////
	input wire [3:0] SD_DAT,	//	SD Card Data
	inout wire SD_CMD,						//	SD Card Command Signal
	output wire SD_CLK,							//	SD Card Clock
	input wire SD_WP_N,							//	SD Card Write Protect
	////////////////////	PS2		////////////////////////////
	input wire PS2_KBCLK,						//	PS2 Keyboard Clock
	input wire PS2_KBDAT,						//	PS2 Keyboard Data
	inout wire PS2_MSCLK,						//	PS2 Mouse Clock
	inout wire PS2_MSDAT,						//	PS2 Mouse Data
	////////////////////	VGA		////////////////////////////
	output wire VGA_HS,						//	VGA H_SYNC
	output wire VGA_VS,						//	VGA V_SYNC
	output wire [3:0] VGA_R,	//	VGA Red[3:0]
	output wire [3:0] VGA_G,	//	VGA Green[3:0]
	output wire [3:0] VGA_B,	//	VGA Blue[3:0]
	////////////////////	GPIO	////////////////////////////
	input wire [1:0] GPIO0_CLKIN,	//	GPIO Connection 0 Clock In Bus
	output wire [1:0] GPIO0_CLKOUT,	//	GPIO Connection 0 Clock Out Bus
	inout wire [31:0] GPIO0_D,		//	GPIO Connection 0 Data Bus
	input wire [1:0] GPIO1_CLKIN,		//	GPIO Connection 1 Clock In Bus
	output wire [1:0] GPIO1_CLKOUT,	//	GPIO Connection 1 Clock Out Bus
	inout wire [31:0] GPIO1_D		//	GPIO Connection 1 Data Bus
);

wire reset, g_reset;

	sys_reset RSTU (
		.RSTn(BUTTON[0]), .CLK(CLOCK_50), .DOUT(reset)
	);

	GLOBAL rst_GU (
		.IN(reset), .OUT(g_reset)
	);

	vjtag_test VU (
		.p_reset(g_reset), //: in std_logic;
		.m_clock(CLOCK_50), //: in std_logic;
		.SW(SW),
		.HEX0(HEX0_D),//	: out std_logic_vector(6 downto 0);	--	Seven Segment Digit 0
		.HEX1(HEX1_D),//	: out std_logic_vector(6 downto 0);	--	Seven Segment Digit 1
		.HEX2(HEX2_D),//	: out std_logic_vector(6 downto 0);	--	Seven Segment Digit 2
		.HEX3(HEX3_D),//	: out std_logic_vector(6 downto 0);	--	Seven Segment Digit 3
		.LEDG(LEDG), //: out std_logic_vector(9 downto 0))
	);

	assign HEX0_DP = 1'b1;
	assign HEX1_DP = 1'b1;
	assign HEX2_DP = 1'b1;
	assign HEX3_DP = 1'b1;

	assign GPIO0_D = 32'bz;
	assign GPIO1_D = 32'bz;

endmodule
