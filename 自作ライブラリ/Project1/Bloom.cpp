#include "Bloom.h"

#include "Texture.h"
#include "Vector.h"
#include"DirectXLib.h"
#include "Input.h"
#include "PipelineState.h"
Window* Bloom::window = nullptr;
Bloom::Bloom()
{
	HRESULT result;
	
	vertices[0].uv = { 0.0f,1.0f };
	vertices[1].uv = { 0.0f,0.0f };
	vertices[2].uv = { 1.0f,1.0f };
	vertices[3].uv = { 1.0f,0.0f };
	vertices[0].pos = { -1.0f,-1.0f,0.0f };
	vertices[1].pos = { -1.0f,+1.0f,0.0f };
	vertices[2].pos = { +1.0f,-1.0f,0.0f };
	vertices[3].pos = { +1.0f,+1.0f,0.0f };

	auto dev = DirectXLib::GetInstance()->GetDevice();

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

void Bloom::Initialize()
{
	HRESULT result;
	auto dev = DirectXLib::GetInstance()->GetDevice();

	auto heapDesc = DirectXLib::GetInstance()->GetRtvHeaps()->GetDesc();

	//使ってるバックバッファーの情報を利用する
	auto& bbuff = DirectXLib::GetInstance()->GetBackBuffers()[0];
	auto resDesc = bbuff->GetDesc();

	D3D12_HEAP_PROPERTIES heapProp =
		CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,D3D12_MEMORY_POOL_L0);

	//レンダリング時のクリア値と同じ値
	float clsClr[4] = { 0.1f,0.6f,0.3f,1.0f };
	D3D12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clsClr);

	for (int i = 0; i < 2; i++)
	{
		//リソース作成
		result = dev->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(peraResource[i].ReleaseAndGetAddressOf()));
		if (result != S_OK)
			assert(0);

	}
	//RTV用ヒープを作る
	heapDesc.NumDescriptors = 4;
	result = dev->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(peraRTVHeap.ReleaseAndGetAddressOf()));
	if (result != S_OK)
		assert(0);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	for (int i = 0; i < 2; i++)
	{
		//レンダーターゲットビューを作る
		dev->CreateRenderTargetView(
			peraResource[i].Get(),
			&rtvDesc,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
			peraRTVHeap->GetCPUDescriptorHandleForHeapStart(),i,
			dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)));
	}
	//SRV用ヒープを作る
	heapDesc.NumDescriptors = 4;
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

	for (int i = 0; i < 2; i++)
	{
		//シェーダーリソースビューを作る
		dev->CreateShaderResourceView(
			peraResource[i].Get(),
			&srvDesc,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				peraSRVHeap->GetCPUDescriptorHandleForHeapStart(),i,
				dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV))
			);
	}

//ブルームバッファの作成
	//黒でクリア
	float clsClr2[4] = { 0,0,0,1.0f };
	D3D12_CLEAR_VALUE clearValue2 = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clsClr2);

	for (auto& res : bloomBuffer)
	{
		result = dev->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue2,
			IID_PPV_ARGS(res.ReleaseAndGetAddressOf())
		);
		assert(SUCCEEDED(result));
		resDesc.Width >>= 1;
	}

	//1つ目RT3枚、２つ目RT1枚
	//3枚目(ブルーム用RTV: ペラポリゴン1)
	dev->CreateRenderTargetView(bloomBuffer[0].Get(), &rtvDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE(
		peraRTVHeap->GetCPUDescriptorHandleForHeapStart(), 2,
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)));
	//3つ目(ブルームテクスチャ)
	dev->CreateShaderResourceView(bloomBuffer[0].Get(), &srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			peraSRVHeap->GetCPUDescriptorHandleForHeapStart(), 2,
			dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV))
	);
	//4枚目(ブルーム用縮小RTV)
	dev->CreateRenderTargetView(bloomBuffer[1].Get(), &rtvDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE(
		peraRTVHeap->GetCPUDescriptorHandleForHeapStart(), 3,
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)));
	//4つ目(高輝度縮小テクスチャ)
	dev->CreateShaderResourceView(bloomBuffer[1].Get(), &srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			peraSRVHeap->GetCPUDescriptorHandleForHeapStart(), 3,
			dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV))
	);

}

void Bloom::DrawShrinkTextureForBlur()
{
	auto dev = DirectXLib::GetInstance()->GetDevice();
	auto cmdList = DirectXLib::GetInstance()->GetCommandList();
	PipelineState::SetPipeline("Bloom");
	//頂点バッファセット
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	//リソースバリア変更
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(bloomBuffer[0].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));


	//リソースバリア変更
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(bloomBuffer[1].Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET));

	auto rtvHandle = peraRTVHeap->GetCPUDescriptorHandleForHeapStart();
	auto srvHandle = peraSRVHeap->GetGPUDescriptorHandleForHeapStart();

	//4つ目に移動
	rtvHandle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * 3;

	auto dsvH = DirectXLib::GetInstance()->GetDsvHeap()->GetCPUDescriptorHandleForHeapStart();
	//レンダーターゲットセット
	cmdList->OMSetRenderTargets(1, &rtvHandle, false, &dsvH);
	float clearColor[] = { 0,0,0,1.0f };
	cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);


	//テクスチャ1枚目の3つ目のレンダーターゲット
	srvHandle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 2;

	//1パス目高輝度をテクスチャとして使用
		//デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { peraSRVHeap.Get() };
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	cmdList->SetGraphicsRootDescriptorTable(1, srvHandle);

	auto desc = bloomBuffer[0]->GetDesc();
	D3D12_VIEWPORT vp = {};
	D3D12_RECT sr = {};

	vp.MaxDepth = 1.0f;
	vp.MinDepth = 0.0f;
	vp.Height = desc.Height / 2;
	vp.Width = desc.Width / 2;
	sr.top = 0;
	sr.left = 0;
	sr.right = vp.Width;
	sr.bottom = vp.Height;
	for (int i = 0; i < 3; ++i)
	{
		cmdList->RSSetViewports(1, &vp);
		cmdList->RSSetScissorRects(1, &sr);
		cmdList->DrawInstanced(4, 1, 0, 0);

		//書いたら下にずらして次を書く準備
		sr.top += vp.Height;
		vp.TopLeftX = 0;
		vp.TopLeftY = sr.top;

		//幅も高さも半分に
		vp.Width /= 2;
		vp.Height /= 2;
		sr.bottom = sr.top + vp.Height;
	}

	//縮小バッファーをシェーダーリソースに
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(bloomBuffer[1].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

}

void Bloom::PreDraw()
{
	auto dev = DirectXLib::GetInstance()->GetDevice();
	auto cmdList = DirectXLib::GetInstance()->GetCommandList();
	for (int i = 0; i < 2; i++)
	{
		//リソースバリア変更
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(peraResource[i].Get(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET));
	}
		//リソースバリア変更
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(bloomBuffer[0].Get(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET));
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapPointers[3];
	for (int i = 0; i < 3; i++)
	{
		rtvHeapPointers[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			peraRTVHeap->GetCPUDescriptorHandleForHeapStart(), i,
			dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		);
	}

	CD3DX12_VIEWPORT viewports[3];
	CD3DX12_RECT scissorRects[3];

	for (int i = 0; i < 3; i++)
	{
		viewports[i] = CD3DX12_VIEWPORT(0.0f, 0.0f, (float)window->GetWindowWidth(), (float)window->GetWindowHeight());
		scissorRects[i] = CD3DX12_RECT(0, 0, window->GetWindowWidth(), window->GetWindowHeight());
	}

	auto dsvH = DirectXLib::GetInstance()->GetDsvHeap()->GetCPUDescriptorHandleForHeapStart();
	cmdList->OMSetRenderTargets(3, rtvHeapPointers, true, &dsvH);
	cmdList->RSSetViewports(3, viewports);
	cmdList->RSSetScissorRects(3, scissorRects);



	//全画面クリア          R    G     B    A
	float clearColor[] = { 0.1f,0.6f,0.3f,1.0f };
	//cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	for (int i = 0; i < 2; i++)
	{
		cmdList->ClearRenderTargetView(rtvHeapPointers[i], clearColor, 0, nullptr);
	}
	float clearColor2[] = {0,0,0,1.0f };
	cmdList->ClearRenderTargetView(rtvHeapPointers[2], clearColor2, 0, nullptr);

	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void Bloom::PostDraw()
{
	auto cmdList = DirectXLib::GetInstance()->GetCommandList();
	for (int i = 0; i < 2; i++)
	{
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(peraResource[i].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
	}
	DrawShrinkTextureForBlur();

}

void Bloom::Draw()
{

	auto dev = DirectXLib::GetInstance()->GetDevice();
	
	auto cmdList = DirectXLib::GetInstance()->GetCommandList();

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
	PipelineState::SetPipeline("Bloom");

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { peraSRVHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	//インデックスバッファのセットコマンド
	cmdList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(peraSRVHeap->GetGPUDescriptorHandleForHeapStart(),0,
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
	
		//2個目のSRVはブルームでは使用していないから消しても良い
	cmdList->SetGraphicsRootDescriptorTable(2, CD3DX12_GPU_DESCRIPTOR_HANDLE(peraSRVHeap->GetGPUDescriptorHandleForHeapStart(),1,
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	cmdList->SetGraphicsRootDescriptorTable(3, CD3DX12_GPU_DESCRIPTOR_HANDLE(peraSRVHeap->GetGPUDescriptorHandleForHeapStart(), 2,
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	cmdList->SetGraphicsRootDescriptorTable(4, CD3DX12_GPU_DESCRIPTOR_HANDLE(peraSRVHeap->GetGPUDescriptorHandleForHeapStart(), 3,
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	
	cmdList->DrawInstanced(4, 1, 0, 0);

}
