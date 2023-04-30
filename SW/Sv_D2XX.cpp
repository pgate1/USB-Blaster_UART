
#if (_MSC_VER >= 1915)
#define no_init_all deprecated
#endif

#include<windows.h>
#pragma comment(lib, "winmm.lib")

#include<stdio.h>
#include<time.h>
#include<memory>

#include "D2XX\\ftd2xx.h"
#pragma comment(lib, "D2XX\\ftd2xx.lib")

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned long  uint32;

const uint16 L = 0x2D2C;
const uint16 H = L | 0x1010;
const uint16 TMS = L | 0x0202;
const uint16 TMS_H = TMS | H;
const uint16 OFF = 0x0D0C;
const uint16 WR = 0x0081;
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
	uint16 buf[] = {(uint16)(((uint16)drCodeValue << 8) | WR)};
	FT_Write(ftHandle, buf, sizeof(buf), &send_size);
}

void WriteShiftir(FT_HANDLE ftHandle, uint8 irCodeValue)
{
	uint32 send_size;
	uint16 buf[] = {(uint16)(((uint16)irCodeValue << 8) | WR), L};
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

int send_data(FT_HANDLE ftHandle, const int total_size)
{
	if(total_size==0) return -1;

	MoveIdle(ftHandle);
	MoveIdleToShiftir(ftHandle);
	WriteShiftir(ftHandle, 0x0E); // USER1
	MoveShiftirToShiftdr(ftHandle);
	WriteShiftdr(ftHandle, 0x41); // カウンタリセットのため(FPGA recv)
	MoveShiftdrToShiftir(ftHandle);
	WriteShiftir(ftHandle, 0x0C); // USER0
	MoveShiftirToShiftdr(ftHandle);

	int wdata_num = total_size; // 送信サイズ
	uint16 *send_buf = new uint16[wdata_num];
	uint8 sum = 0;
	for(int i=0; i<wdata_num; i++){
		uint8 data = rand();
	//	printf("0x%02X ", data);
		sum += data;
		send_buf[i] = ((uint16)data << 8) | WR;
	}

DWORD st, et;
st = timeGetTime();

	uint32 send_size;
	FT_Write(ftHandle, send_buf, wdata_num*2, &send_size);

et = timeGetTime();
int u = et - st;
double s = u / 1000.0;

	delete[] send_buf;

printf("\n");
printf("send sum 0x%02X\n", sum);
printf("send %dms %0.1f kB/s\n\n", u, wdata_num/s/1024.0);

	DeviceClose(ftHandle);

	return 0;
}

int recv_data(FT_HANDLE ftHandle, const int total_size)
{
	if(total_size==0) return -1;

	MoveIdle(ftHandle);
	MoveIdleToShiftir(ftHandle);
	WriteShiftir(ftHandle, 0x0E); // USER1
	MoveShiftirToShiftdr(ftHandle);
	WriteShiftdr(ftHandle, 0x42); // カウンタリセットのため(FPGA send)
	MoveShiftdrToShiftir(ftHandle);
	WriteShiftir(ftHandle, 0x0C); // USER0
	MoveShiftirToShiftdr(ftHandle);

	uint16 send_buf[64];
	for(int i=0; i<64; i++) send_buf[i] = L;

	int rdata_num = 0x3F; // 0x3Fが最大
	uint8 *recv_buf = new uint8[rdata_num];

	send_buf[0] = RD | rdata_num;

DWORD st, et;
st = timeGetTime();

	uint8 sum = 0;
	int t = 0;
	for(int d=0; d<total_size; d+=rdata_num){

		uint32 send_size;
		FT_Write(ftHandle, send_buf, rdata_num+1, &send_size); // max 64
		uint32 recv_size;
		FT_Read(ftHandle, recv_buf, rdata_num, &recv_size);

		for(int i=0; i<(int)recv_size && t<total_size; i++,t++){
			sum += recv_buf[i];
			//printf("0x%02X ", recv_buf[i]);
		}
	}

et = timeGetTime();
int u = et - st;
double s = u / 1000.0;

	delete[] recv_buf;

printf("\n");
printf("recv sum 0x%02X\n", sum);
printf("recv %dms %0.1f kB/s\n", u, total_size/s/1024.0);

	DeviceClose(ftHandle);

	return 0;
}

int main(void)
{
	const int total_size = 3;
	printf("size %d byte\n", total_size);

	FT_STATUS ftStatus;
	FT_HANDLE ftHandle;

	ftStatus = FT_OpenEx((PVOID)"USB-Blaster", FT_OPEN_BY_DESCRIPTION, &ftHandle);
	if(ftStatus!=FT_OK){
		printf("Failed to open port\n");
		return 0;
	}

	srand((uint32)time(NULL));

	send_data(ftHandle, total_size);

	FT_Close(ftHandle);

//	getchar();

	ftStatus = FT_OpenEx((PVOID)"USB-Blaster", FT_OPEN_BY_DESCRIPTION, &ftHandle);
	if(ftStatus!=FT_OK){
		printf("Failed to open port\n");
		return 0;
	}

	recv_data(ftHandle, total_size);

	FT_Close(ftHandle);

//	getchar();
	return 0;
}
