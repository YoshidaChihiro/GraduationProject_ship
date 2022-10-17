#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include "ComputeShade.h"

class ComputeWrapper
{
public:
	static ComputeWrapper* GetInstance();
	void Initialize();
	void AddShade(ComputeShade* shade) { shades.push_back(shade); }
	void End();
	ID3D12GraphicsCommandList* GetCmdList() { return cmdList.Get(); }
	//void MoveToNextFrame();
private:
	ComputeWrapper() = default;
	~ComputeWrapper() = default;
	ComputeWrapper(const ComputeWrapper&) = delete;
	ComputeWrapper& operator=(const ComputeWrapper&) = delete;

	//void Create();
	//static DWORD WINAPI ThreadProc(ComputeWrapper* data)
	//{
	//	return data->AsyncComputeThreadProc();
	//}
	void AsyncComputeThreadProc();
	std::vector<ComputeShade*> shades;
private:
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator;
	Microsoft::WRL::ComPtr < ID3D12CommandQueue> cmdQueue;
	Microsoft::WRL::ComPtr < ID3D12GraphicsCommandList> cmdList;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	volatile HANDLE fenceEvent;
	HANDLE threadHandle;
	UINT64 volatile m_threadFenceValue = 0;
	LONG volatile m_terminating = 0;

	ID3D12Device* dev;

	friend class DirectXLib;
};

