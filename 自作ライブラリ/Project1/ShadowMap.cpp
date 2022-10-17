#include "ShadowMap.h"
#include "Texture.h"
#include "Vector.h"
#include"DirectXLib.h"
#include "Input.h"
#include "PipelineState.h"
Window* ShadowMap::window = nullptr;
ShadowMap::ShadowMap()
{
	HRESULT result;
	dev = DirectXLib::GetInstance()->GetDevice();
	cmdList = DirectXLib::GetInstance()->GetCommandList();

	vertices[0].uv = { 0.0f,1.0f };
	vertices[1].uv = { 0.0f,0.0f };
	vertices[2].uv = { 1.0f,1.0f };
	vertices[3].uv = { 1.0f,0.0f };
	vertices[0].pos = { -1.0f,-1.0f,0.0f };
	vertices[1].pos = { -1.0f,+1.0f,0.0f };
	vertices[2].pos = { +1.0f,-1.0f,0.0f };
	vertices[3].pos = { +1.0f,+1.0f,0.0f };

	//頂点バッファの生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertBuff)
	);
	assert(SUCCEEDED(result));

	//頂点バッファを転送
	VERTEX* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result))
	{
		memcpy(vertMap, vertices.data(), sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

	//頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);

	//定数バッファの作成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDate) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));

	Initialize();
}

void ShadowMap::Initialize()
{
	HRESULT result;

	auto heapDesc = DirectXLib::GetInstance()->GetRtvHeaps()->GetDesc();

	//使ってるバックバッファーの情報を利用する
	auto& bbuff = DirectXLib::GetInstance()->GetBackBuffers()[0];
	auto resDesc = bbuff->GetDesc();
	resDesc.Format = DXGI_FORMAT_R32_FLOAT;
	D3D12_HEAP_PROPERTIES heapProp =
		CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	resDesc.Height = resourceHeight;
	resDesc.Width = resourceWidth;

	//レンダリング時のクリア値と同じ値
	float clsClr[4] = { 0.0f,1.0f,1.0f,1.0f };
	D3D12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R32_FLOAT, clsClr);

	//リソース作成
	result = dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
		IID_PPV_ARGS(shadowResource.ReleaseAndGetAddressOf()));
	if (result != S_OK)
		assert(0);

	//RTV用ヒープを作る
	heapDesc.NumDescriptors = 1;
	result = dev->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(peraRTVHeap.ReleaseAndGetAddressOf()));
	if (result != S_OK)
		assert(0);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R32_FLOAT;

	//レンダーターゲットビューを作る
	dev->CreateRenderTargetView(
		shadowResource.Get(),
		&rtvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			peraRTVHeap->GetCPUDescriptorHandleForHeapStart(), 0,
			dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)));

	//SRV用ヒープを作る
	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;


	result = dev->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(peraSRVHeap.ReleaseAndGetAddressOf()));
	if (result != S_OK)
		assert(0);


	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	//シェーダーリソースビューを作る
	dev->CreateShaderResourceView(
		shadowResource.Get(),
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			peraSRVHeap->GetCPUDescriptorHandleForHeapStart(), 0,
			dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV))
	);

	//深度値用ヒーププロパティ
	//D3D12_HEAP_PROPERTIES depthHeapProp{};
	//depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	//リソース設定
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		resourceWidth,
		resourceHeight,
		1, 0, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	//リソース設定
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),//アップロード不可
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度値書き込みに使用
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuffer));
	assert(SUCCEEDED(result));

	//深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;//深度ビューは1つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//デプスステンシルビュー
	result = dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
	assert(SUCCEEDED(result));

	//深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart());

	
	Texture::AddTexture("shadowMap", shadowResource.Get());
}

void ShadowMap::PreDraw()
{
	//リソースバリア変更
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(shadowResource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET));

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapPointer;
	rtvHeapPointer = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		peraRTVHeap->GetCPUDescriptorHandleForHeapStart(), 0,
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
	);

	CD3DX12_VIEWPORT viewport;
	CD3DX12_RECT scissorRect;

	viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, (float)window->GetWindowWidth(), (float)window->GetWindowHeight());
	scissorRect = CD3DX12_RECT(0, 0, window->GetWindowWidth(), window->GetWindowHeight());

	auto dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->OMSetRenderTargets(1, &rtvHeapPointer, true, &dsvH);
	cmdList->RSSetViewports(1, &viewport);
	cmdList->RSSetScissorRects(1, &scissorRect);



	//全画面クリア          R    G     B    A
	float clearColor[] = { 0.0f,1.0f,1.0f,1.0f };
	//cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	cmdList->ClearRenderTargetView(rtvHeapPointer, clearColor, 0, nullptr);

	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void ShadowMap::PostDraw()
{
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(shadowResource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

}

void ShadowMap::Draw()
{

	const XMFLOAT4 color = { 1,1,1,1 };

	HRESULT result;

	ConstBufferDate* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (result != S_OK)
		assert(0);

	constMap->mat = XMMatrixIdentity();
	constMap->color = color;
	constBuff->Unmap(0, nullptr);

	//パイプラインのセット
	PipelineState::SetPipeline("ShadowMap");

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { Texture::GetBasicDescHeap().Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	//インデックスバッファのセットコマンド
	cmdList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(Texture::GetBasicDescHeap().Get()->GetGPUDescriptorHandleForHeapStart(), Texture::GetTexIndex("shadowMap"),
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));


	cmdList->DrawInstanced(4, 1, 0, 0);

}
