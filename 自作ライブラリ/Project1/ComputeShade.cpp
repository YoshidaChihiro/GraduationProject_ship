#include "ComputeShade.h"
#include <d3dcompiler.h>

#include "ComputeWrapper.h"
#include "DirectXLib.h"
#include "Object3D.h"
#include "PipelineState.h"

ComPtr<ID3D12RootSignature> ComputeShade::rootsignature = {};
ComPtr<ID3D12PipelineState> ComputeShade::pipeline = {};
ID3D12Device* ComputeShade::dev = nullptr;
ID3D12GraphicsCommandList* ComputeShade::cmdList = nullptr;

ComputeShade::ComputeShade(const int numElement):maxElement(numElement)
{
	CreateInputResource();
	CreateOutputResource();
	CreateDescHeap();
}
void ComputeShade::StaticInitialize()
{
	dev = DirectXLib::GetInstance()->GetDevice();
	//cmdList = DirectXLib::GetInstance()->GetCommandList();

	cmdList = ComputeWrapper::GetInstance()->GetCmdList();
	CreatePipeline();
}

OutputData* ComputeShade::GetOutputData()
{
	OutputData* outputData2 = (OutputData*)outputData;
	return outputData2;
}


void ComputeShade::Update(void* sendInputData, void* sendInputData2,const int element)
{
	if(sendInputData != nullptr)
	memcpy(inputData1, sendInputData, sizeof(ParticleParameter) * element);
	if (sendInputData2 != nullptr)
	memcpy(inputData2, sendInputData2, sizeof(OutputData) * element);
	this->element = element;
}

void ComputeShade::Dispatch()
{
	//if(Object3D::GetDrawShadow())
	//	return;
	if (element <= 0)
		return;
	cmdList->SetComputeRootSignature(rootsignature.Get());
	cmdList->SetPipelineState(pipeline.Get());
	
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	CD3DX12_GPU_DESCRIPTOR_HANDLE handle;

	handle = descHeap->GetGPUDescriptorHandleForHeapStart();
	cmdList->SetComputeRootDescriptorTable(0, handle);
	handle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	cmdList->SetComputeRootDescriptorTable(1, handle);
	handle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	cmdList->SetComputeRootDescriptorTable(2, handle);
	if(element==0)
	{
		return;
	}
	cmdList->Dispatch(element, 1, 1);
	element = 0;
	//OutputData* outputData2 = (OutputData*)outputData;

	//ImGui::Begin("ComputeShade");

	//for(int i = 0;i<maxElement;i++)
	//{
	//	ImGui::Text("InputPosition {%f,%f,%f}\n", outputData2[i].position.x, outputData2[i].position.y, outputData2[i].position.z);
	//}
	//ImGui::Text("InputPosition {%f,%f,%f}\n", sendInputData[0].position.x, sendInputData[0].position.y, sendInputData[0].position.z);
	//ImGui::Text("outputPosition {%f,%f,%f}\n", outputData2[0].position.x, outputData2[0].position.y, outputData2[0].position.z);
	//ImGui::Text("InputPosition {%f,%f,%f}\n", sendInputData[1].position.x, sendInputData[1].position.y, sendInputData[1].position.z);
	//ImGui::Text("outputPosition {%f,%f,%f}\n", outputData2[1].position.x, outputData2[1].position.y, outputData2[1].position.z);

	//ImGui::End();

	//sendInputData[0].position = outputData2[0].position;
	//sendInputData[1].position = outputData2[1].position;

}


void ComputeShade::CreateInputResource()
{
	HRESULT result;

	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(ParticleParameter) * maxElement);
	D3D12_RESOURCE_DESC desc2 = CD3DX12_RESOURCE_DESC::Buffer(sizeof(OutputData) * maxElement);

	D3D12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	result = dev->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&inputResource1)
	);
	assert(SUCCEEDED(result));

	
	//構造化バッファをCPUからアクセス可能な仮想アドレス空間にマッピングする。
	//マップ、アンマップのオーバーヘッドを軽減するためにはこのインスタンスが生きている間は行わない。
	{
		CD3DX12_RANGE readRange(0, 0);        //     intend to read from this outputResource on the CPU.
		inputResource1.Get()->Map(0, &readRange, reinterpret_cast<void**>(&inputData1));
	}

	result = dev->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc2,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&inputResource2)
	);
	assert(SUCCEEDED(result));
	//構造化バッファをCPUからアクセス可能な仮想アドレス空間にマッピングする。
	//マップ、アンマップのオーバーヘッドを軽減するためにはこのインスタンスが生きている間は行わない。
	{
		CD3DX12_RANGE readRange(0, 0);        //     intend to read from this outputResource on the CPU.
		inputResource2.Get()->Map(0, &readRange, reinterpret_cast<void**>(&inputData2));
	}

}

void ComputeShade::CreateOutputResource()
{
	HRESULT result;

	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(OutputData)* maxElement);
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	
	D3D12_HEAP_PROPERTIES prop{};
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	prop.CreationNodeMask = 1;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	prop.Type = D3D12_HEAP_TYPE_CUSTOM;
	prop.VisibleNodeMask = 1;

	result = dev->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&outputResource)
	);
	assert(SUCCEEDED(result));

	//構造化バッファをCPUからアクセス可能な仮想アドレス空間にマッピングする。
	//マップ、アンマップのオーバーヘッドを軽減するためにはこのインスタンスが生きている間は行わない。
	{
		CD3DX12_RANGE readRange(0, 0);        //     intend to read from this outputResource on the CPU.
		outputResource.Get()->Map(0, &readRange, reinterpret_cast<void**>(&outputData));
	}


}

void ComputeShade::CreateDescHeap()
{
	//DescriptorHeapの作成
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.NumDescriptors = 3;

		auto result = dev->CreateDescriptorHeap(
			&heapDesc,
			IID_PPV_ARGS(descHeap.ReleaseAndGetAddressOf()));
		assert(SUCCEEDED(result));
	}
	
	auto cpuHandle = descHeap->GetCPUDescriptorHandleForHeapStart();
	auto gpuHandle = descHeap->GetGPUDescriptorHandleForHeapStart();

	//SRVの作成
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = maxElement;
		srvDesc.Buffer.StructureByteStride = sizeof(ParticleParameter);
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		dev->CreateShaderResourceView(
			inputResource1.Get(),
			&srvDesc,
			cpuHandle
		);
		cpuHandle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	//SRVの作成
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = maxElement;
		srvDesc.Buffer.StructureByteStride = sizeof(OutputData);
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		dev->CreateShaderResourceView(
			inputResource2.Get(),
			&srvDesc,
			cpuHandle
		);
		cpuHandle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	//UAVの作成
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC heapDesc2 = {};
		heapDesc2.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		heapDesc2.Format = DXGI_FORMAT_UNKNOWN;
		heapDesc2.Buffer.NumElements = maxElement;
		heapDesc2.Buffer.StructureByteStride = sizeof(OutputData);


		dev->CreateUnorderedAccessView(
			outputResource.Get(), nullptr,
			&heapDesc2,
			cpuHandle);
		cpuHandle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
}


void ComputeShade::CreatePipeline()
{

	HRESULT result;
	ComPtr < ID3DBlob> csBlob = nullptr; //コンピュートシェーダオブジェクト
	ComPtr < ID3DBlob> errorBlob = nullptr; //エラーオブジェクト


	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shader/ComputeShader.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"CSmain", "cs_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&csBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	CD3DX12_DESCRIPTOR_RANGE1 ranges[3];
	CD3DX12_ROOT_PARAMETER1 rootParameters[3];

	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
	ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
	rootParameters[0].InitAsDescriptorTable(1, &ranges[0]);
	rootParameters[1].InitAsDescriptorTable(1, &ranges[1]);
	rootParameters[2].InitAsDescriptorTable(1, &ranges[2]);

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);


	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error);
	assert(SUCCEEDED(result));

	
	//ルートシグネチャの生成
	result = dev->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
		IID_PPV_ARGS(&rootsignature));
	assert(SUCCEEDED(result));

	//コンピュートシェーダー用のパイプラインの設定
	D3D12_COMPUTE_PIPELINE_STATE_DESC cpipeline{};
	cpipeline.CS = CD3DX12_SHADER_BYTECODE(csBlob.Get());
	//cpipeline.CS.pShaderBytecode = csBlob.Get()->GetBufferPointer();
	//cpipeline.CS.BytecodeLength = csBlob.Get()->GetBufferSize();
	cpipeline.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	cpipeline.NodeMask = 0;
	
	//パイプラインにルートシグネチャをセット
	cpipeline.pRootSignature = rootsignature.Get();

	result = dev->CreateComputePipelineState(&cpipeline, IID_PPV_ARGS(&pipeline));
	assert(SUCCEEDED(result));
}

