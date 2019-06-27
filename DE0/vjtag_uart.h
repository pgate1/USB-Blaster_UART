
// vjtag_uart.h

declare vjtag_uart
{
	instrout init_recv, recv;
	output recv_data<8>;

	instrout init_send, send;
	instrin send_set;
	input send_data<8>;

	instr_arg send_set(send_data);
}
