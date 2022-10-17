#pragma once

#include <DirectXMath.h>
#include <d3d12.h>
#include"DirectXTex.h"
#include"Input.h"
#include<wrl.h>
#include"Camera.h"
#include"DirectXLib.h"
using namespace Microsoft::WRL;
using namespace DirectX;

#define DIRECTINPUT_VERSION 0x0800 //DirectInputのバージョン

class DirectXLib;
class Create3DObject
{

public:
	Create3DObject();
	~Create3DObject();
	virtual void CreatePyramid(const int vertex, const float topHeight, const int R, XMFLOAT3 scale, XMFLOAT3 rot, XMFLOAT3 trans, bool projective, XMFLOAT4 color, ID3D12Resource* texbuff, TexMetadata& metadata);
	virtual void CreateBox(const int vertex, const float TopHeight, const int Width, XMFLOAT3 scale, XMFLOAT3 rot, XMFLOAT3 trans, bool projective, XMFLOAT4 color, ID3D12Resource* texbuff, TexMetadata& metadata);
	virtual void DrawPyramid(XMFLOAT4 color,ID3D12PipelineState* pipelinestate, ID3D12RootSignature * rootsignature);
	virtual void DrawBox(XMFLOAT4 color, ID3D12PipelineState* pipelinestate, ID3D12RootSignature * rootsignature);
	XMFLOAT3 GetPosition();
	virtual void Move();
	virtual void Update();
	virtual void SetDirection();
	//virtual void Hit(Create3DObject* object);
	virtual void Damege();
	bool GetIsDead();
	void Die();
	float GetR();
	int GetWidth();
protected:
	Camera* camera;
	DirectXLib* device;
	Input* input;
	const int window_width = 1280;//横幅
	const int window_height = 720;//横幅
	float r;
	bool isDead = false;
	int vertex;
	int height, width;
	int hp;
	bool isDamege = false;
	XMFLOAT4 baseColor;
	int damegeCounter = 20;
	XMFLOAT3 basePosition;
	XMFLOAT3 baseRot;
	XMMATRIX baseScale;

	struct Vertex
	{
		XMFLOAT3 pos;//xyz座標
		XMFLOAT3 normal;//法線ベクトル
		XMFLOAT2 uv; //uv座標
	};
	struct ConstBufferDate {
		XMFLOAT4 color;//色(RGBA)
		XMMATRIX mat;//3D変換行列
	};
	Vertex vertices[200];
	unsigned short indices[600];
	XMMATRIX matWorld;
	XMMATRIX matScale;//スケーリング行列
	XMMATRIX matRot;//回転行列
	XMFLOAT3 rotation = { 0.0f,0.0f,0.0f };
	XMMATRIX matTrans;//平行移動行列
	XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };//座標を変数として持つ


	ComPtr<ID3D12Resource> vertBuff;
	//GPU上のバッファに対応した仮想メモリを取得
	Vertex* vertMap = nullptr;
	//インデックスバッファビューの作成とセット
	D3D12_INDEX_BUFFER_VIEW ibView{};
	ConstBufferDate* constMap = nullptr;
	XMMATRIX matProjection; //射影変換行列
	ComPtr<ID3D12Resource> indexBuff;
	ComPtr<ID3D12Resource> constBuff;
	ComPtr<ID3D12Resource> depthBuffer;

	//頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	ComPtr<ID3D12DescriptorHeap> basicDescHeap;

	//2つのビューのGPU側のアドレスを記録する
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleCBV;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;

};

