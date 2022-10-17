#include "TextureResource.h"

#include "DirectXLib.h"
#include "Texture.h"
std::vector<TextureResource*> TextureResource::nowRenderTargets = {};
TextureResource::TextureResource(const std::string& name, const bool noDepth)
	: resourceWidth(1920), resourceHeight(1080), format(DXGI_FORMAT_R8G8B8A8_UNORM), clearColor{ 0,0,0,0 }, noDepth(noDepth)
{
	Initialize(name);
}

TextureResource::TextureResource(const std::string& name, const bool noDepth, const bool changeTex, const Vector2& size)
	: resourceWidth(size.x), resourceHeight(size.y), format(DXGI_FORMAT_R8G8B8A8_UNORM), clearColor{ 0.254f,0.58f,1,1 }, noDepth(noDepth),changeTex(changeTex)
{
	Initialize(name);
}

TextureResource::TextureResource(const std::string& name, const Vector2& size, const DXGI_FORMAT resourceFormat, const DirectX::XMFLOAT4& arg_clearColor, const bool noDepth)
	: resourceWidth(size.x), resourceHeight(size.y), format(resourceFormat), clearColor{ arg_clearColor.x,arg_clearColor.y, arg_clearColor.z, arg_clearColor.w }, noDepth(noDepth)
{
	Initialize(name);
}
void TextureResource::Initialize(const std::string& name)
{
	dev = DirectXLib::GetInstance()->GetDevice();
	cmdList = DirectXLib::GetInstance()->GetCommandList();
	auto heapDesc = DirectXLib::GetInstance()->GetRtvHeaps()->GetDesc();

	//使ってるバックバッファーの情報を利用する
	auto& bbuff = DirectXLib::GetInstance()->GetBackBuffers()[0];
	auto resDesc = bbuff->GetDesc();
	resDesc.Format = format;
	D3D12_HEAP_PROPERTIES heapProp =
		CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	resDesc.Height = resourceHeight;
	resDesc.Width = resourceWidth;

	//レンダリング時のクリア値と同じ値
	D3D12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(format, clearColor.data());

	HRESULT result;

	result = dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
		IID_PPV_ARGS(resource.ReleaseAndGetAddressOf()));
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
	rtvDesc.Format = format;

	dev->CreateRenderTargetView(
		resource.Get(),
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

	dev->CreateShaderResourceView(
		resource.Get(),
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			peraSRVHeap->GetCPUDescriptorHandleForHeapStart(), 0,
			dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV))
	);
	//深度値用ヒーププロパティ
	//D3D12_HEAP_PROPERTIES depthHeapProp{};
	//depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	if (!noDepth)
	{
		//リソース設定
		CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_R32_TYPELESS,
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
	}
	if (changeTex)
	{
		Texture::ChangeTexture(name, resource.Get());
		return;
	}
		Texture::AddTexture(name, resource.Get());
}

void TextureResource::PreDraw(const UINT arg_numRTD, const float topLeftX, const float topLeftY, const float width, const float height,
	const LONG& left, const LONG& top, const LONG& right, const LONG& bottom)
{
	numRTD = arg_numRTD;
	nowRenderTargets.push_back(this);

	//リソースバリア変更
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET));
	if (numRTD == 1)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapPointer;
		rtvHeapPointer = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			peraRTVHeap->GetCPUDescriptorHandleForHeapStart(), 0,
			dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		);
		D3D12_CPU_DESCRIPTOR_HANDLE dsvH;
		CD3DX12_VIEWPORT viewport;
		CD3DX12_RECT scissorRect;

		viewport = CD3DX12_VIEWPORT(topLeftX, topLeftY, width, height);
		scissorRect = CD3DX12_RECT(left, top, right, bottom);
		dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
		cmdList->OMSetRenderTargets(1, &rtvHeapPointer, true, &dsvH);
		cmdList->RSSetViewports(1, &viewport);
		cmdList->RSSetScissorRects(1, &scissorRect);

		cmdList->ClearRenderTargetView(rtvHeapPointer, clearColor.data(), 0, nullptr);

		cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	}
	else
	{
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHeapPointer;
		rtvHeapPointer.resize(arg_numRTD);

		auto it = nowRenderTargets.end();
		it--;

		for (int i = arg_numRTD - 1; i >= 0; i--)
		{
			rtvHeapPointer[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(
				(*it)->peraRTVHeap->GetCPUDescriptorHandleForHeapStart(), 0,
				dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
			if (it == nowRenderTargets.begin())
				break;
			it--;
		}

		std::vector <CD3DX12_VIEWPORT> viewports;
		std::vector <CD3DX12_RECT> scissorRects;
		viewports.resize(arg_numRTD);
		scissorRects.resize(arg_numRTD);

		for (int i = 0; i < arg_numRTD; i++)
		{
			viewports[i] = CD3DX12_VIEWPORT(topLeftX, topLeftY, width, height);
			scissorRects[i] = CD3DX12_RECT(left, top, right, bottom);
		}

		auto dsvH = (*it)->dsvHeap->GetCPUDescriptorHandleForHeapStart();
		cmdList->OMSetRenderTargets(arg_numRTD, rtvHeapPointer.data(), false, &dsvH);
		cmdList->RSSetViewports(arg_numRTD, viewports.data());
		cmdList->RSSetScissorRects(arg_numRTD, scissorRects.data());

		cmdList->ClearRenderTargetView(rtvHeapPointer[arg_numRTD - 1], clearColor.data(), 0, nullptr);

		//cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	}
}

void TextureResource::DirectPreDraw(const UINT arg_numRTD, const float topLeftX, const float topLeftY, const float width, const float height, const LONG& left, const LONG& top, const LONG& right, const LONG& bottom)
{
	numRTD = arg_numRTD;
	nowRenderTargets.push_back(this);

	//リソースバリア変更
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET));
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHeapPointer;
	rtvHeapPointer.resize(arg_numRTD);

	auto it = nowRenderTargets.end();
	it--;

	for (int i = arg_numRTD - 1; i >= 1; i--)
	{
		rtvHeapPointer[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			(*it)->peraRTVHeap->GetCPUDescriptorHandleForHeapStart(), 0,
			dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		if (it == nowRenderTargets.begin())
			break;
		it--;
	}
	rtvHeapPointer[0] = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		DirectXLib::GetInstance()->GetRtvHeaps()->GetCPUDescriptorHandleForHeapStart(), 0,
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));


	std::vector <CD3DX12_VIEWPORT> viewports;
	std::vector <CD3DX12_RECT> scissorRects;
	viewports.resize(arg_numRTD);
	scissorRects.resize(arg_numRTD);

	for (int i = 0; i < arg_numRTD; i++)
	{
		viewports[i] = CD3DX12_VIEWPORT(topLeftX, topLeftY, width, height);
		scissorRects[i] = CD3DX12_RECT(left, top, right, bottom);
	}

	auto dsvH = DirectXLib::GetInstance()->GetDsvHeap()->GetCPUDescriptorHandleForHeapStart();
	cmdList->OMSetRenderTargets(arg_numRTD, rtvHeapPointer.data(), false, &dsvH);
	cmdList->RSSetViewports(arg_numRTD, viewports.data());
	cmdList->RSSetScissorRects(arg_numRTD, scissorRects.data());

	cmdList->ClearRenderTargetView(rtvHeapPointer[arg_numRTD - 1], clearColor.data(), 0, nullptr);
}

void TextureResource::DepthClear()
{
	auto dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

}


void TextureResource::ResetRenderTarget()
{
	auto rtvH =
		CD3DX12_CPU_DESCRIPTOR_HANDLE(peraRTVHeap->GetCPUDescriptorHandleForHeapStart(), 0, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	auto dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, resourceWidth, resourceHeight));
	cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, resourceWidth, resourceHeight));

	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

}

void TextureResource::PostDraw(const bool renderTargetReset)
{

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
	nowRenderTargets.pop_back();
	if (!renderTargetReset)
		return;
	if (nowRenderTargets.size() == 0)
	{
		DirectXLib::GetInstance()->ReRenderTarget();
	}
	else
	{
		auto it = nowRenderTargets.end();
		it--;
		(*it)->ResetRenderTarget();
	}
	//if (mainResource != this && numRTD == 1)
	//	mainResource->ResetRenderTarget();

}

