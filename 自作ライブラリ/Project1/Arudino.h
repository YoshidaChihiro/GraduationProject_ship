#pragma once
#include <windows.h>
class Arudino
{
public:
	Arudino();
	void Initialize();
	int ReceiveData();
	void SendData();
	void End();
private:
	HANDLE Portarduino;
	bool ready;
	UINT8 receiveData[2];
	UINT8 sendData = 0;
};

