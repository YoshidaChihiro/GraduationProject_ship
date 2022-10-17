#pragma once
#include <array>
#include<DirectXMath.h>
#include<d3d12.h>
#include <wrl.h>
#include<d3dx12.h>
#include"DirectionalLight.h"
#include"PointLight.h"
#include"SpotLight.h"
#include"CircleShadow.h"
#include "Vector.h"

class LightGroup
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public://静的定数
	//平光源の数
	static const int DirLightNum = 3;
	//点光源の数
	static const int PointLightNum = 9;
	//スポットライトの数
	static const int SpotLightNum = 3;
	//丸影の数
	static const int CircleShadowNum = 1;
public:
	//定数バッファ用構造体
	struct ConstBufferData
	{
		XMFLOAT3 ambientColor;
		float pad;//パディング
		std::array<DirectionalLight::ConstBuff,DirLightNum> dirLights;
		std::array<PointLight::ConstBuff, PointLightNum> pointLights;
		std::array<SpotLight::ConstBuff, SpotLightNum> spotLights;
		//丸影用
		std::array<CircleShadow::ConstBuff, CircleShadowNum> circleShadows;
	};
public://静的メンバ関数

	static LightGroup* Create();

public://メンバ関数
	void Initialize();
	void TransferConstBuff();
	//平行光源
	void SetAmbientColor(const Vector3& color);
	void SetDirLightActive(int index, bool active);
	void SetDirLightDir(const int& index, const XMVECTOR& lightdir);
	void SetDirLightColor(const int& index, const Vector3 lightcolor);
	//点光源
	void SetPointLightActive(int index, bool active);
	void SetPointLightPos(int index, const Vector3& pos);
	void SetPointLightColor(int index, const Vector3& color);
	void SetPointLightAtten(int index, const Vector3& atten);
	//スポットライト
	void SetSpotLightActive(int index, bool active);
	void SetSpotLightDir(int index, const XMVECTOR& dir);
	void SetSpotLightPos(int index, const Vector3& pos);
	void SetSpotLightColor(int index, const Vector3& color);
	void SetSpotLightAtten(int index, const Vector3& atten);
	void SetSpotLightFactorAngle(int index, const XMFLOAT2& lightFactorAngle);
	//丸影
	void SetCircleShadowActive(int index, bool active);
	void SetCircleShadowCasterPos(int index, const Vector3& pos);
	void SetCircleShadowDir(int index, const XMVECTOR& dir);
	void SetCircleShadowDistanceCasterLight(int index, float distanceCasterLight);
	void SetCircleShadowAtten(int index, const Vector3& atten);
	void SetCircleShadowFactorAngle(int index, const XMFLOAT2& factorAngle);

	void Update();
	void Draw(UINT rootParameterIndex);

private://メンバ変数
	ComPtr<ID3D12Resource> constBuff;//定数バッファ

	XMFLOAT3 ambientColor{ 1,1,1 };
	//平行光源の配列
	std::array<DirectionalLight, DirLightNum> dirLights;
	//点光源の配列
	std::array<PointLight, PointLightNum> pointLights;
	//スポットライトの配列
	std::array<SpotLight, SpotLightNum> spotLights;
	//丸影の配列
	std::array<CircleShadow, CircleShadowNum> circleShadows;
	bool dirty = false;

};
