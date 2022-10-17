#pragma once
#include "Object.h"
#include "TouchAbleObject.h"

class Grass :
    public TouchAbleObject
{
public:
	Grass();
	void Draw() override;
private:
	void CreateConstBuff();
	struct ConstBuffData
	{
		XMFLOAT3 playerPos;
		float pad;
		XMFLOAT4 GrassBottomColor;
		XMFLOAT4 GrassTopColor;
		XMFLOAT2 AirDir;
		float AirPower;
		int Time;

		float playerRadius;
		float GrassWidth;
		float GrassHeight;
	};
	struct ConstLightCameraBuff
	{
		XMMATRIX viewProjection;
		XMFLOAT3 cameraPos;
	};
	ComPtr<ID3D12Resource> constBuff; // 定数バッファ
	ComPtr<ID3D12Resource> constCameraBuff; // 定数バッファ

	float GrassWidth;
	float GrassHeight;
	float airDir[2];
	float airPower;
	int time;

};

