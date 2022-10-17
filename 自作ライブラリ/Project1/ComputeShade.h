#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include<vector>

#include "ParticleParameter.h"
#include"Vector.h"


class ComputeShade
{
public:
	ComputeShade(const int numElement);
	void Update(void* sendInputData1, void* sendInputData2,const int element);
	void Dispatch();
	static ID3D12PipelineState* GetPipeline() { return pipeline.Get(); }
private:
	void CreateInputResource();
	void CreateOutputResource();
	void CreateDescHeap();
	Microsoft::WRL::ComPtr<ID3D12Resource>outputResource;
	Microsoft::WRL::ComPtr<ID3D12Resource>inputResource1;
	Microsoft::WRL::ComPtr<ID3D12Resource>inputResource2;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap;
	void* outputData = nullptr;
	void* inputData1 = nullptr;
	void* inputData2 = nullptr;

	int element;
	//std::vector<InputData> sendInputData;

	int maxElement;
public://静的メンバ関数
	static void StaticInitialize();
	//template <typename T>
	//T get;
	OutputData* GetOutputData();

private://静的メンバ変数・関数

	static void CreatePipeline();
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> rootsignature;
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> pipeline;
	static ID3D12Device* dev;
	static ID3D12GraphicsCommandList* cmdList;
};

