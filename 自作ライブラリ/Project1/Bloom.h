#pragma once
#include <array>
#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl/client.h>

#include "window.h"

class Bloom {
public:
	Bloom();
	static void SetWindow(Window* window) { Bloom::window = window; };
	void PreDraw();
	void PostDraw();
	void Draw();
private:
	void Initialize();
	void DrawShrinkTextureForBlur();
	static Window* window;

	//シェーダ―用
	struct ConstBufferDate {
		DirectX::XMFLOAT4 color;
		DirectX::XMMATRIX mat;
	};
	struct VERTEX
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
	};

	std::array<VERTEX,4> vertices;
	D3D12_VERTEX_BUFFER_VIEW vbView{};//頂点バッファビュー
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;//頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;//定数バッファ
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> peraResource;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> peraRTVHeap;//レンダーターゲット用
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> peraSRVHeap;//テクスチャ用

	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> bloomBuffer;//ブルーム用バッファ

};
