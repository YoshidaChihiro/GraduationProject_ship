#pragma once
#include <array>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <DirectXMath.h>
#include <wrl.h>
#include "d3dx12.h"
#include<imgui.h>
#include <stdexcept>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


using namespace Microsoft::WRL;
class Window;
class DirectXLib
{
private:
	//friend class Bloom;
	DirectXLib() = default;
	DirectXLib(const DirectXLib&) = delete;
	~DirectXLib() = default;
	DirectXLib& operator=(const DirectXLib&) = delete;

	static const int FrameCount = 2;
	
	Window* window;

	ComPtr < ID3D12GraphicsCommandList> cmdList;
	ComPtr<ID3D12Device> dev;
	HRESULT CreateDXGI();
	HRESULT CreateCommands();
	HRESULT CreateSwapChain();
	HRESULT CreateDesc();
	HRESULT CreateFence();	
	HRESULT CreateDSV();

	HRESULT ImguiInitialize();

	std::vector< ComPtr <IDXGIAdapter>> adapters;
	ComPtr<IDXGIFactory6> dxgiFactory;
	ComPtr<IDXGISwapChain4 > swapchain;
	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	ComPtr < ID3D12CommandQueue> cmdQueue;
	ComPtr < ID3D12DescriptorHeap> rtvHeaps;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr < ID3D12Resource> depthBuffer;

	ComPtr<ID3D12DescriptorHeap> imguiHeap;

	std::vector< ComPtr<ID3D12Resource>> backBuffers;
	ComPtr < ID3D12Fence> fence;
	UINT64 fenceVal;

	UINT bbIndex;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH;
public:
	static DirectXLib* GetInstance();
	
	void Initialize(Window* window);
	inline  ID3D12Device* GetDevice() { return dev.Get(); }
	inline  ID3D12GraphicsCommandList* GetCommandList() { return cmdList.Get(); }
	ID3D12CommandQueue* GetCmdQueue() { return cmdQueue.Get(); }
	inline const UINT& GetBbIndex() { return bbIndex; }
	std::vector< ComPtr<ID3D12Resource>>& GetBackBuffers() { return backBuffers; }
	ComPtr<ID3D12DescriptorHeap>& GetDsvHeap() { return dsvHeap; }
	ComPtr < ID3D12DescriptorHeap>& GetRtvHeaps() { return  rtvHeaps; }
	ComPtr < ID3D12Fence>& GetFence() { return fence; }
	UINT64& GetCurrentFenceVal() { return fenceVal; }
	
	void ImguiDraw();
	void BeginDraw();
	void DepthClear();
	void EndDraw();
	void End();
	void ReRenderTarget();
	void ComputeBegin();
};

