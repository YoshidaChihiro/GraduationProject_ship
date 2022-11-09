#include "Arudino.h"
#include <stdio.h>
#include <stdlib.h>

HANDLE Arudino::Portarduino = {};

bool Arudino::ready = false;
UINT8 Arudino::receiveData[] = {};
//UINT8 Arudino::sendData = {};

//超音波センサ
UINT16 Arudino::datas_ultrasonic[] = {};

//マイクロスイッチ
UINT8 Arudino::datas_microSwitch = 0;
bool Arudino::isPressed_prev = false;


//Arudino::Arudino()
//{
//}

void Arudino::Initialize()
{
	Portarduino = CreateFile(L"\\\\.\\COM3", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);//ポート設定
	if (Portarduino == INVALID_HANDLE_VALUE)
	{
		DWORD lastError = GetLastError();
		printf("NG1: error=%lu, 0x%08X\n", lastError, lastError);
		system("PAUSE");
		exit(0);
	}

	//バッファ初期化
	ready = SetupComm(Portarduino, 1024, 1024);
	if (!ready) {
		printf("NG2\n");
		CloseHandle(Portarduino);
		system("PAUSE");
		exit(0);
	}

	ready = PurgeComm(Portarduino, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!ready) {
		printf("NG3\n");
		CloseHandle(Portarduino);
		exit(0);
	}

	DCB dcb;
	GetCommState(Portarduino, &dcb);
	dcb.DCBlength = sizeof(DCB);
	dcb.BaudRate = 115200;
	dcb.fBinary = TRUE;
	dcb.ByteSize = 8;
	dcb.fParity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;

	ready = SetCommState(Portarduino, &dcb);
	if (!ready) {
		printf("NG4\n");
		CloseHandle(Portarduino);
		system("PAUSE");
		exit(0);
	}

}

void Arudino::ReceiveData()
{
	//受信待機
	DWORD dwRead;
	ready = ReadFile(Portarduino, receiveData, sizeof(UINT8) * allDataNum_receive, &dwRead, NULL);
	if (!ready) {
		printf("NG5\n");
	}

	//超音波センサ
	UINT16 data1 = (UINT16)(receiveData[0] << 8) | (UINT16)receiveData[1];
	UINT16 data2 = (UINT16)(receiveData[2] << 8) | (UINT16)receiveData[3];
	printf("data1 : %u\n", data1);
	printf("data2 : %u\n", data2);
	printf("\n");
	datas_ultrasonic[0] = data1;
	datas_ultrasonic[1] = data2;

	//マイクロスイッチ
	isPressed_prev = datas_microSwitch;
	datas_microSwitch = receiveData[4];
}

void Arudino::SendData()
{
	//if (receiveData == '1')
	//{
	//	DWORD dwSendSize;
	//	DWORD dwErrorMask;
	//	sendData = 1;
	//	COMSTAT portComStat;    /* 通信状態バッファ */
	//	ClearCommError(Portarduino, &dwErrorMask, &portComStat);
	//	ready = WriteFile(Portarduino, &sendData, sizeof(sendData), &dwSendSize, NULL);
	//	if (!ready) {
	//		printf("SEND FAILED\n");
	//		CloseHandle(Portarduino);
	//	}
	//}
	//else if (receiveData == '0')
	//{
	//	sendData = 2;
	//	DWORD dwSendSize;
	//	DWORD dwErrorMask;
	//	COMSTAT portComStat;    /* 通信状態バッファ */
	//	ClearCommError(Portarduino, &dwErrorMask, &portComStat);

	//	ready = WriteFile(Portarduino, &sendData, sizeof(sendData), &dwSendSize, NULL);
	//	if (!ready) {
	//		printf("SEND FAILED\n");
	//		CloseHandle(Portarduino);
	//	}
	//}
}

void Arudino::End()
{
	CloseHandle(Portarduino);
}

UINT16 Arudino::GetData_ultrasonic(const int arg_arrayNum)
{
	return datas_ultrasonic[arg_arrayNum];
}

bool Arudino::GetData_microSwitch()
{
	return datas_microSwitch;
}

bool Arudino::GetData_microSwitch_Trigger()
{
	return !isPressed_prev && datas_microSwitch;
}
