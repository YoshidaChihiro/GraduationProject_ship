#pragma once

#include<DirectXMath.h>

#include "Vector.h"

class DirectionalLight
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
		XMFLOAT3 lightcolor;
		float pad;
	};

public://メンバ関数
	void SetLightDir(const XMVECTOR& lightdir);

	inline const XMVECTOR& GetLightDir() { return lightdir; }

	void SetLightColor(const Vector3& lightcolor);

	inline const Vector3& GetLightColor() { return lightcolor; }

	inline void SetActive(bool active) { this->active = active; }

	inline bool IsActive() { return active; }
	
private:

	XMVECTOR lightdir = { 1,0,0,0 };
	Vector3 lightcolor = { 1,1,1 };
	bool dirty = false;
	bool active = false;
};

