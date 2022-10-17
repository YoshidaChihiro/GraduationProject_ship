#pragma once
#include <d3d12.h>
#include <string>
#include <wrl/client.h>

#include "DirectXLib.h"
#include "Vector.h"

class TextureResource
{
public:
	TextureResource(const std::string& name, const bool noDepth);
	TextureResource(const std::string& name, const bool noDepth,const bool changeTex, const Vector2& size);
	TextureResource(const std::string& name, const Vector2& size = { 1920,1080 }, const DXGI_FORMAT resourceFormat = DXGI_FORMAT_R8G8B8A8_UNORM, const DirectX::XMFLOAT4& arg_clearColor = { 0,0,0,0 }, const bool noDepth = false);

	void PostDraw(const bool renderTargetReset = true);
	void PreDraw(const UINT arg_numRTD = 1, const float topLeftX = 0, const float topLeftY = 0, const float width = 1920, const float height = 1080,
		const LONG& left = 0, const LONG& top = 0, const LONG& right = 1920, const LONG& bottom = 1080);
	void DirectPreDraw(const UINT arg_numRTD, const float topLeftX = 0, const float topLeftY = 0, const float width = 1920, const float height = 1080,
		const LONG& left = 0, const LONG& top = 0, const LONG& right = 1920, const LONG& bottom = 1080);

	void DepthClear();
private:
	void Initialize(const std::string& name);
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	Microsoft::WRL::ComPtr <ID3D12Resource> depthBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> peraRTVHeap;//レンダーターゲット用
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> peraSRVHeap;//テクスチャ用
	ID3D12GraphicsCommandList* cmdList;
	ID3D12Device* dev;
	DXGI_FORMAT format;
	std::array<float,4> clearColor;
	int resourceWidth;
	int resourceHeight;
	UINT numRTD = 1;
	void ResetRenderTarget();
	bool noDepth;
	bool changeTex = false;
private:
	static std::vector<TextureResource*> nowRenderTargets;
	static TextureResource* mainResource;
public:
	static void SetMainResource(TextureResource* ptr)
	{
		mainResource = ptr;
	}
private:
	friend class SSAO;
	friend class DeferredRendering;

};

