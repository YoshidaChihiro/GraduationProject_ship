#pragma once
#include <windows.h>
class Arudino
{
public:
	Arudino();
	void Initialize();
	void ReceiveData();
	void SendData();
	void End();

	UINT16 GetData(const int arg_arrayNum);

private:
	HANDLE Portarduino;
	bool ready;
	UINT8 receiveData[4];
	UINT8 sendData = 0;

	UINT16 datas[2];
};

