/*
	VirtualJTAGでデータ送受信
*/

module vjtag_uart (
	p_reset, m_clock,
	init_recv, recv, recv_data,
	init_send, send, send_set, send_data
);

	input p_reset, m_clock;
	output init_recv, recv;
	output [7:0] recv_data;
	output init_send, send;
	input send_set;
	input [7:0] send_data;

	wire [7:0] ir_in;
	wire tck, tdi;
	wire tdo;
	wire virtual_state_sdr, virtual_state_uir;
	reg [7:0] ir, dr, ds;
	reg [2:0] count;

	reg t_init;
	reg [1:0] m_init;
	reg t_recv;
	reg [1:0] m_recv;
	reg t_send;
	reg [2:0] m_send;

	virtual_jtag vjtag (
		.ir_out(8'h00),
		.tdo(tdo),
		.ir_in(ir_in),
		.tck(tck),
		.tdi(tdi),
		.virtual_state_sdr(virtual_state_sdr),
		.virtual_state_uir(virtual_state_uir)
	);

	always @(posedge p_reset or posedge tck) begin
		if(p_reset) begin
			t_init <= 0;
			t_recv <= 0;
			t_send <= 0;
		end
		else if(virtual_state_uir) begin
			count <= 0;
			t_init <= 1;
			t_recv <= 0;
			ir <= ir_in;
			if(ir_in==8'h42)
				t_send <= 1;
		end
		else if(virtual_state_sdr) begin
			t_init <= 0;
			dr[count] <= tdi;
			count <= count + 1;
			if(count==7) begin
				if(ir==8'h41)
					t_recv <= 1;
				else
					t_send <= 1;
			end
			else begin
				if(ir==8'h41)
					t_recv <= 0;
				else
					t_send <= 0;
			end
		end
	end

	// t_init同期化
	always @(posedge p_reset or posedge m_clock) begin
		if(p_reset)
			m_init <= 2'b00;
		else
			m_init <= {m_init[0], t_init};
	end

	// 立上がり検出
	assign init_recv = (ir_in==8'h41 && m_init==2'b01) ? 1 : 0;
	assign init_send = (ir_in==8'h42 && m_init==2'b01) ? 1 : 0;

	// t_recv同期化
	always @(posedge p_reset or posedge m_clock) begin
		if(p_reset)
			m_recv <= 2'b00;
		else
			m_recv <= {m_recv[0], t_recv};
	end

	assign recv_data = dr;
	assign recv = m_recv==2'b01 ? 1 : 0;

	// t_send同期化
	always @(posedge p_reset or posedge m_clock) begin
		if(p_reset)
			m_send <= 3'b000;
		else
			m_send <= {m_send[1:0], t_send};
	end

	assign send = m_send==3'b011 ? 1 : 0;

	always @(posedge p_reset or posedge m_clock) begin
		if(p_reset)
			ds <= 8'h00;
		else if(send_set)
			ds <= send_data;
	end

	assign tdo = ds[count];

endmodule
