#pragma once
#include <windows.h>
class Arudino
{
public:
	//Arudino();
	static void Initialize();
	static void ReceiveData();
	static void SendData();
	static void End();

	static UINT16 GetData_ultrasonic(const int arg_arrayNum);

	static bool GetData_microSwitch();
	static bool GetData_microSwitch_Trigger();

private:
	static HANDLE Portarduino;

	static bool ready;
	static const int allDataNum_receive = 5;
	static UINT8 receiveData[allDataNum_receive];
	static UINT8 sendData;

	//超音波センサ
	static UINT16 datas_ultrasonic[2];

	//マイクロスイッチ
	static UINT8 datas_microSwitch;
	static bool isPressed_prev;

};

