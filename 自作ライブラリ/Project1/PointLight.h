#pragma once
#include<DirectXMath.h>

class PointLight
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
		XMFLOAT3 lightpos;
		float pad1;//パディング
		XMFLOAT3 lightcolor;
		float pad2;//パディング
		XMFLOAT3 lightatten;
		UINT active;
	};
public://メンバ関数
	inline void SetLightPos(const Vector3& pos){
		this->lightpos = pos;}
	inline const Vector3& GetLightPos(){ return lightpos; }

	inline void SetLightColor(const Vector3&  color) {
		this->lightcolor = color;}
	inline const Vector3& GetLightColor() { return lightcolor; }

	inline void SetLightAtten(const Vector3& atten) {
		this->lightAtten = atten;
	}
	inline const Vector3& GetLightAtten() { return lightAtten; }
	inline void SetActive(bool active) { this->active = active; }
	inline bool IsActive() { return active; }

private://メンバ変数
	Vector3 lightpos = { 0,0,0 };//ライト座標(ワールド座標系)
	Vector3 lightcolor = { 1,1,1 };//ライトの色
	Vector3 lightAtten = { 0.001f,0.001f,0.001f };//ライトの距離減衰係数
	bool active = false;
};
