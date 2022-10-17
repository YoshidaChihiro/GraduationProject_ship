#pragma once
#include<DirectXMath.h>

class SpotLight
{
private:
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public://バッファ用データ
	struct ConstBuff
	{
		XMVECTOR lightv;
		XMFLOAT3 lightpos;
		float pad1;//パディング
		XMFLOAT3 lightcolor;
		float pad2;//パディング
		XMFLOAT3 lightatten;
		float pad3;//パディング
		XMFLOAT2 lightfactoranglecos;
		unsigned int active;
		float pad4;//パディング
	};
public://メンバ関数
	inline void SetLightDir(const XMVECTOR& dir) { this->lightdir = DirectX::XMVector3Normalize(dir); }
	inline const XMVECTOR& GetLightDir() { return lightdir; }

	inline void SetLightPos(const Vector3& pos) {
		this->lightpos = pos;}
	inline const Vector3& GetLightPos() { return lightpos; }

	inline void SetLightColor(const Vector3&  color) {
		this->lightcolor = color;}
	inline const Vector3& GetLightColor() { return lightcolor; }

	inline void SetLightAtten(const Vector3& atten) {
		this->lightAtten = atten;}
	inline const Vector3& GetLightAtten() { return lightAtten; }

	inline void SetLightFactorAngle(const XMFLOAT2& lightFactorAngle)
	{
		this->lightFactorAngleCos.x = cosf(DirectX::XMConvertToRadians(lightFactorAngle.x));
		this->lightFactorAngleCos.y = cosf(DirectX::XMConvertToRadians(lightFactorAngle.y));
	}
	inline const XMFLOAT2 GetLightFactorAngleCos() { return lightFactorAngleCos; }

	inline void SetActive(bool active) { this->active = active; }
	inline bool IsActive() { return active; }

private://メンバ変数
	XMVECTOR lightdir = { 1,0,0,0 };//ライト方向(単位ベクトル)
	Vector3 lightpos = { 0,0,0 };//ライト座標(ワールド座標系)
	Vector3 lightcolor = { 1,1,1 };//ライトの色
	Vector3 lightAtten = { 1.0f,1.0f,1.0f };//ライトの距離減衰係数
	XMFLOAT2 lightFactorAngleCos = { 0.5f,0.2f };//ライト減衰角度(開始角度、終了角度)
	bool active = false;



};