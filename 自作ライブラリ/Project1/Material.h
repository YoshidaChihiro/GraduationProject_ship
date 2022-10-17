#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>

class Material
{
public:
	Material();
	~Material();

private://省略用
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://定数バッファ用構造体
	struct ConstBufferData
	{
		XMFLOAT3 ambient;
		float pad1;
		XMFLOAT3 diffuse;
		float pad2;
		XMFLOAT3 specular;
		float alpha;
	};

public://マテリアル情報(パブリックでいいのか？)
	std::string name;
	XMFLOAT3 ambient;
	XMFLOAT3 diffuse;
	XMFLOAT3 specular;
	float alpha;
	std::string textureFileName;


public://静的メンバ関数
	static Material* Create();//生成


public://メンバ関数
	void Initialize();//初期化
	ID3D12Resource* GetConstBuffer() { return constBuff.Get(); };
	void LoadTexture();
	void Update();


private://GPUとのやり取りで必要な変数	
	ComPtr<ID3D12Resource> texbuff;//テクスチャバッファ
	ComPtr<ID3D12Resource> constBuff;//定数バッファ
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;//シェーダーリソースビューハンドルCPU
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;//シェーダーリソースビューハンドルGPU
};

