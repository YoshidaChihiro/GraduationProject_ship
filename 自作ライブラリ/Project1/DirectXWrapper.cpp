#include "DirectXWrapper.h"
#include"window.h"
#include"PtrDelete.h"
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include "PostEffect.h"

DirectXWrapper* DirectXWrapper::GetInstance()
{
	static DirectXWrapper instance;
	return &instance;
}

void DirectXWrapper::Initialize(Window* window)
{
	this->window = window;
	if (CreateDXGI() != S_OK) assert(0);
	if (CreateCommands() != S_OK) assert(0);
	if (CreateSwapChain() != S_OK) assert(0);
	if (CreateDesc() != S_OK) assert(0);
	if (CreateDSV() != S_OK) assert(0);
	//if(CreatePeraResurce() != S_OK)assert(0);
	if (CreateFence() != S_OK) assert(0);
	if (ImguiInitialize() != S_OK)assert(0);
}

HRESULT DirectXWrapper::CreateDXGI()
{
	HRESULT result;

	//DXGI ファクトリーの生成
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	if (result != S_OK) return result;

	// アダプターの列挙用
	//ここに特定の名前を持つアダプターオブジェクトが入る
	ComPtr < IDXGIAdapter> tmpAdapter;
	for (int i = 0; dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(tmpAdapter);//動的配列に追加する
	}
	for (int i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC adesc{};
		adapters[i]->GetDesc(&adesc);  //アダプターの情報を取得
		std::wstring strDesc = adesc.Description;  //アダプター名
		//Microsoft Basic Render Diver を回避
		if (strDesc.find(L"Microsoft") == std::wstring::npos)
		{
			tmpAdapter = adapters[i];  //採用
			break;
		}
	}


	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;


	for (int i = 0; i < _countof(levels); i++)
	{
		// デバイスを生成
		result = D3D12CreateDevice(nullptr, levels[i], IID_PPV_ARGS(&dev));
		if (result == S_OK)
		{
			// デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}
	return result;
}

HRESULT DirectXWrapper::CreateCommands()
{
	HRESULT result;
	//コマンドアロケータを作成
	result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator));
	if (result != S_OK) return result;

	//コマンドリストを作成
	result = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&cmdList));
	if (result != S_OK) return result;

	//標準設定でコマンドキューを生成
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));

	return result;
}

HRESULT DirectXWrapper::CreateSwapChain()
{
	HRESULT result;
	//各種設定をしてスワップチェーンを生成
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = window->GetWindowWidth();
	swapchainDesc.Height = window->GetWindowHeight();
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  //色情報の書式
	swapchainDesc.SampleDesc.Count = 1; //マルチサンプルしない
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; //バックバッファ用
	swapchainDesc.BufferCount = 2; //バッファ数を2つに設定
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //フリップ後は破棄
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	ComPtr<IDXGISwapChain1> swapchain1;
	result = dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue.Get(), window->GetHwnd(), &swapchainDesc, nullptr, nullptr, (IDXGISwapChain1**)&swapchain1);
	if (result == S_FALSE) return result;

	swapchain1.As(&swapchain);

	return result;
}

HRESULT DirectXWrapper::CreateDesc()
{
	HRESULT result;
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = swapchain->GetDesc(&swcDesc);
	if (FAILED(result)) {
		assert(0);
		return result;
	}


	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	//各種設定をしてディスクリプタヒープを生成
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; //レンダーターゲットビュー
	heapDesc.NumDescriptors = swcDesc.BufferCount;   //裏表の2つ
	result = dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
	if (result != S_OK) return result;

	//裏表の2つに分について
	backBuffers.resize(swcDesc.BufferCount);
	for (int i = 0; i < backBuffers.size(); i++)
	{
		//スワップチェーンからバッファを取得
		result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		if (result != S_OK) return result;
		//ディスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			rtvHeaps->GetCPUDescriptorHandleForHeapStart(),
			i,
			dev->GetDescriptorHandleIncrementSize(heapDesc.Type));

		//レンダーターゲットビューの作成
		dev->CreateRenderTargetView(backBuffers[i].Get(), nullptr, handle);
	}
	return result;
}

HRESULT DirectXWrapper::CreateFence()
{
	HRESULT result;
	//フェンスの生成
	fence = nullptr;
	fenceVal = 0;
	result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	return result;
}

HRESULT DirectXWrapper::CreateDSV()
{

	HRESULT result;

	//深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	//リソース設定
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		window->GetWindowWidth(),
		window->GetWindowHeight(),
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
	if (result != S_OK) return result;

	//深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;//深度ビューは1つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//デプスステンシルビュー
	result = dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
	if (result != S_OK) return result;

	//深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

HRESULT DirectXWrapper::ImguiInitialize()
{
	HRESULT result = S_FALSE;

	// デスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.NumDescriptors = 1;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	result = dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&imguiHeap));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// スワップチェーンの情報を取得
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = swapchain->GetDesc(&swcDesc);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	if (ImGui::CreateContext() == nullptr) {
		assert(0);
		return result;
	}
	if (!ImGui_ImplWin32_Init(window->GetHwnd())) {
		assert(0);
		return result;
	}
	if (!ImGui_ImplDX12_Init(
		GetDevice(),
		swcDesc.BufferCount,
		swcDesc.BufferDesc.Format,
		imguiHeap.Get(),
		imguiHeap->GetCPUDescriptorHandleForHeapStart(),
		imguiHeap->GetGPUDescriptorHandleForHeapStart()
	)) {
		assert(0);
		return result;
	}

	return result;
}

void DirectXWrapper::ImguiDraw()
{
	// imgui開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void DirectXWrapper::BeginDraw()
{
	//バックバッファの番号を取得(2つなので0番か1番)
	bbIndex = swapchain->GetCurrentBackBufferIndex();
	// 1.リソースバリアを変更
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),  D3D12_RESOURCE_STATE_PRESENT,D3D12_RESOURCE_STATE_RENDER_TARGET));

	//2.画面クリアコマンドここから
	//auto rtvHeapPointer = CD3DX12_CPU_DESCRIPTOR_HANDLE(peraRTVHeap->GetCPUDescriptorHandleForHeapStart(), bbIndex, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	//レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
	rtvH =
		CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(), bbIndex, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	//cmdList->OMSetRenderTargets(1, &rtvH, false, nullptr);
	//深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, (float)window->GetWindowWidth(), (float)window->GetWindowHeight()));
	cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, window->GetWindowWidth(), window->GetWindowHeight()));
	dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	//cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);


	
	//全画面クリア          R    G     B    A
	float clearColor[] = { 0.1f,0.6f,0.3f,1.0f };
	//cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

}

void DirectXWrapper::Depth()
{
	//auto rtvHeapPointer = peraRTVHeap->GetCPUDescriptorHandleForHeapStart();
	//dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	//cmdList->OMSetRenderTargets(1, &rtvHeapPointer, false, &dsvH);

}

void DirectXWrapper::EndDraw()
{
	//cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(peraResource.Get(),
	//	D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	//dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	//cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);
	//postEffect->Draw(peraSRVHeap.Get(), peraSRVHeap->GetGPUDescriptorHandleForHeapStart());
	
	// imgui描画
	ImGui::Render();
	ID3D12DescriptorHeap* ppHeaps[] = { imguiHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList.Get());

	//4.リソースバリアを戻す
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	//命令のクローズ
	cmdList->Close();

	//コマンドリストの実行
	ID3D12CommandList* cmdLists[] = { cmdList.Get() };//コマンドリストの配列
	cmdQueue->ExecuteCommandLists(1, cmdLists);
	//コマンドリストの実行完了を待つ
	cmdQueue->Signal(fence.Get(), ++fenceVal);
	if (fence->GetCompletedValue() !=fenceVal)
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
	cmdAllocator->Reset(); //キューをクリア
	cmdList->Reset(cmdAllocator.Get(), nullptr);		//再びコマンドリストを貯める準備
	//バッファをフリップ(裏表の入れ替え)
	swapchain->Present(1, 0);

	// DirectX 毎フレーム処理 ここまで

}

void DirectXWrapper::End()
{
	//PtrDelete(postEffect);
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

