
#if (_MSC_VER >= 1915)
#define no_init_all deprecated
#endif

#include<windows.h>
#pragma comment(lib, "winmm.lib")

#include<stdio.h>
#include<time.h>
#include<memory>

#if _M_AMD64
#include "D2XXx64\\ftd2xx.h"
#pragma comment(lib, "D2XXx64\\ftd2xx.lib")
#else
#include "D2XX\\ftd2xx.h"
#pragma comment(lib, "D2XX\\ftd2xx.lib")
#endif

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned long  uint32;

const uint16 L = 0x2D2C;
const uint16 H = L | 0x1010;
const uint16 TMS = L | 0x0202;
const uint16 TMS_H = TMS | H;
const uint16 OFF = 0x0D0C;
const uint16 WR = 0x0080;
const uint16 RD = 0x00C0;

void MoveIdle(FT_HANDLE ftHandle)
{
	uint32 send_size;
	uint16 buf[] = {TMS, TMS, TMS, TMS, TMS, L};
	FT_Write(ftHandle, buf, sizeof(buf), &send_size);
}

void MoveIdleToShiftir(FT_HANDLE ftHandle)
{
	uint32 send_size;
	uint16 buf[] = {TMS, TMS, L, L};
	FT_Write(ftHandle, buf, sizeof(buf), &send_size);
}

void WriteShiftdr(FT_HANDLE ftHandle, uint8 drCodeValue)
{
	uint32 send_size;
	uint16 buf[] = {(uint16)(((uint16)drCodeValue << 8) | WR | 0x0001)};
	FT_Write(ftHandle, buf, sizeof(buf), &send_size);
}

void WriteShiftir(FT_HANDLE ftHandle, uint8 irCodeValue)
{
	uint32 send_size;
	uint16 buf[] = {(uint16)(((uint16)irCodeValue << 8) | WR | 0x0001), L};
	FT_Write(ftHandle, buf, sizeof(buf), &send_size);
}

void MoveShiftirToShiftdr(FT_HANDLE ftHandle)
{
	uint32 send_size;
	uint16 buf[] = {TMS, TMS, TMS, L, L};
	FT_Write(ftHandle, buf, sizeof(buf), &send_size);
}

void MoveShiftdrToShiftir(FT_HANDLE ftHandle)
{
	uint32 send_size;
	uint16 buf[] = {TMS_H, TMS, TMS, TMS, L, L};
	FT_Write(ftHandle, buf, sizeof(buf), &send_size);
}

void DeviceClose(FT_HANDLE ftHandle)
{
	uint32 send_size;
	uint16 buf[] = {TMS_H, TMS, OFF};
	FT_Write(ftHandle, buf, sizeof(buf), &send_size);
}

uint8 check_sum(uint8 *data, int size)
{
	uint8 sum = 0;
	for(int i=0; i<size; i++){
		sum += data[i];
	//	if((i%16000)==0) printf("0x%02X ", data[i]);
	}
//	printf("0x%02X ", data[size-1]);
	return sum;
}

// 63バイト毎にWRコマンドを送る
// 620 kB/s
int BlasterSend(FT_HANDLE ftHandle, uint8 *send_data, const int send_size)
{
	if(send_data==NULL) return -1;
	if(send_size<=0) return -1;

	MoveIdle(ftHandle);
	MoveIdleToShiftir(ftHandle);
	WriteShiftir(ftHandle, 0x0E); // USER1
	MoveShiftirToShiftdr(ftHandle);
	WriteShiftdr(ftHandle, 0x41); // カウンタリセットのため(FPGA recv)
	MoveShiftdrToShiftir(ftHandle);
	WriteShiftir(ftHandle, 0x0C); // USER0
	MoveShiftirToShiftdr(ftHandle);

	// bytes_to_write=0; for(i=0; i<send_size; i++) bytes_to_write += (i%63)==0 ? 2 : 1;
	DWORD bytes_to_write = ((send_size - 1) / 63) + send_size + 1;

	// バッファサイズは63バイト毎に32ワード
	int write_buf_size = (bytes_to_write + 1) / 2;
	// 32ワードで割り切れるサイズでwrite_bufを確保する
	write_buf_size = ((write_buf_size - 1) / 32 + 1) * 32;
	uint16 *write_buf = new uint16[write_buf_size];
	int last_index = 0;
	for(int i=0, d=0; i<write_buf_size; i+=32, d+=63){
		// 32ワード毎にWRを入れる
		write_buf[i] = WR | 0x003F; // max 63
		memcpy((uint8*)(write_buf + i) + 1, send_data + d, 63);
		// 最後のWR位置を記憶しておく
		last_index = i;
	}
	// 63でワンセットなので63で割った余りを最後のWR位置に入れる。
	uint16 rem = send_size % 63;
	// 余りが0の時はすでに63が入っている。それ以外の場合に余りを入れる。
	if(rem!=0) write_buf[last_index] = (write_buf[last_index] & 0xFF00) | WR | rem;

DWORD st = timeGetTime();

	DWORD bytes_written;
	FT_Write(ftHandle, write_buf, bytes_to_write, &bytes_written);

DWORD et = timeGetTime();
int u = et - st;
double s = u / 1000.0;
printf("send sum 0x%02X\n", check_sum(send_data, send_size));
printf("send %dms %0.1f kB/s\n\n", u, send_size/s/1024.0);

	delete[] write_buf;

	DeviceClose(ftHandle);

	return 0;
}

// 63バイト毎にRDコマンドを送る
// 380 kB/s
// recv_size 254001バイトまで。それを超えると速度が落ちる。
int BlasterRecv(FT_HANDLE ftHandle, uint8 *recv_data, const int recv_size, bool f_SLD_trans)
{
	if(recv_data==NULL) return -1;
	if(recv_size<=0) return -1;

	if(f_SLD_trans){
		MoveIdle(ftHandle);
		MoveIdleToShiftir(ftHandle);
		WriteShiftir(ftHandle, 0x0E); // USER1
		MoveShiftirToShiftdr(ftHandle);
		WriteShiftdr(ftHandle, 0x42); // カウンタリセットのため(FPGA send)
		MoveShiftdrToShiftir(ftHandle);
		WriteShiftir(ftHandle, 0x0C); // USER0
		MoveShiftirToShiftdr(ftHandle);
	}

	// bytes_to_write=0; for(i=0; i<recv_size; i++) bytes_to_write += (i%63)==0 ? 2 : 1;
	DWORD bytes_to_write = ((recv_size - 1) / 63) + recv_size + 1;

	// バッファサイズは63バイト毎に32ワード
	// 偶数ならbytes_to_write/2、奇数ならbytes_to_write+1して/2
	int write_buf_size = (bytes_to_write + 1) / 2;
	uint16 *write_buf = new uint16[write_buf_size];
	int last_index = 0;
	memset(write_buf, 0x00, write_buf_size * 2);
	for(int i=0; i<write_buf_size; i+=32){
		// 32ワード毎にRDを入れる
		write_buf[i] = RD | 0x003F; // max 63
		// 最後のRD位置を記憶しておく
		last_index = i;
	}
	// 63でワンセットなので63で割った余りを最後のRD位置に入れる。
	uint16 rem = recv_size % 63;
	// 余りが0の時はすでに63が入っている。それ以外の場合に余りを入れる。
	if(rem!=0) write_buf[last_index] = RD | rem;

DWORD st = timeGetTime();

	DWORD bytes_written;
	FT_Write(ftHandle, write_buf, bytes_to_write, &bytes_written);

	DWORD bytes_read;
	FT_Read(ftHandle, recv_data, recv_size, &bytes_read);

DWORD et = timeGetTime();
int u = et - st;
double s = u / 1000.0;
if(f_SLD_trans){
printf("recv sum 0x%02X\n", check_sum(recv_data, recv_size));
printf("recv %dms %0.1f kB/s\n\n", u, recv_size/s/1024.0);
}
	delete[] write_buf;

	if(f_SLD_trans){
		DeviceClose(ftHandle);
	}

	return 0;
}

// 受信サイズがRECV_MAXを超える場合に、RECV_MAX毎に受信する。
// 380 kB/s
//#define RECV_MAX (4031 * 63)
#define RECV_MAX (4000 * 63) // 少し余裕を持たせる
// 32MBまで動作確認
int BlasterRecv(FT_HANDLE ftHandle, uint8 *recv_data, const int recv_size)
{
	if(recv_data==NULL) return -1;
	if(recv_size<=0) return -1;

	MoveIdle(ftHandle);
	MoveIdleToShiftir(ftHandle);
	WriteShiftir(ftHandle, 0x0E); // USER1
	MoveShiftirToShiftdr(ftHandle);
	WriteShiftdr(ftHandle, 0x42); // カウンタリセットのため(FPGA send)
	MoveShiftdrToShiftir(ftHandle);
	WriteShiftir(ftHandle, 0x0C); // USER0
	MoveShiftirToShiftdr(ftHandle);

DWORD st = timeGetTime();

	int count = recv_size / RECV_MAX;
	for(int i=0; i<count; i++){
		BlasterRecv(ftHandle, recv_data + RECV_MAX * i, RECV_MAX, false);
	}
	BlasterRecv(ftHandle, recv_data + RECV_MAX * count, recv_size % RECV_MAX, false);

DWORD et = timeGetTime();
int u = et - st;
double s = u / 1000.0;
printf("recv sum 0x%02X\n", check_sum(recv_data, recv_size));
printf("recv %dms %0.1f kB/s\n\n", u, recv_size/s/1024.0);

	DeviceClose(ftHandle);

	return 0;
}

int main(void)
{
	const int size = 32768;
	printf("size %d byte\n\n", size);

/*
	// case send file
	FILE *ifp = fopen("send_data.bin", "rb");
	if(ifp==NULL){
		printf("send_data.bin not found\n");
		return -1;
	}
	uint8 *send_data = new uint8[size];
	fread(send_data, 1, size, ifp);
	fclose(ifp);
*/
	// case send rand
	srand((uint32)time(NULL));
	uint8 *send_data = new uint8[size];
	for(int i=0; i<size; i++){
		send_data[i] = rand();
	}


	FT_STATUS ftStatus;
	FT_HANDLE ftHandle;

	// Send
	ftStatus = FT_OpenEx((PVOID)"USB-Blaster", FT_OPEN_BY_DESCRIPTION, &ftHandle);
	if(ftStatus!=FT_OK){
		printf("Failed to open port\n");
		return -1;
	}
	BlasterSend(ftHandle, send_data, size);
	FT_Close(ftHandle);
	delete[] send_data;

//	getchar();

	// Receive
	ftStatus = FT_OpenEx((PVOID)"USB-Blaster", FT_OPEN_BY_DESCRIPTION, &ftHandle);
	if(ftStatus!=FT_OK){
		printf("Failed to open port\n");
		return -1;
	}
	uint8 *recv_data = new uint8[size];
	BlasterRecv(ftHandle, recv_data, size);
	FT_Close(ftHandle);
/*
	FILE *ofp = fopen("recv_data.bin", "wb");
	fwrite(recv_data, 1, size, ofp);
	fclose(ofp);
*/
	delete[] recv_data;

//	getchar();
	return 0;
}
