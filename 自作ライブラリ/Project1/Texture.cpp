#include "Texture.h"
#include"DirectXLib.h"
std::unordered_map<std::string, TexMetadata> Texture::metadetes = {};
std::unordered_map<std::string, ComPtr <ID3D12Resource> > Texture::texbuffs = {};
ComPtr < ID3D12DescriptorHeap> Texture::basicDescHeape = nullptr;
std::unordered_map<std::string, unsigned char>Texture::texIndexes = {};
unsigned char Texture::texIndex = 0;
std::unordered_map<std::string, XMINT2> Texture::createTexSize = {};

void Texture::LoadTexture(const std::string& name, const std::string& texName)
{
	HRESULT result;
	auto dev = DirectXLib::GetInstance()->GetDevice();
	
	TexMetadata metadata{};
	ScratchImage scratchlmg{};
	// ファイルパスを結合
	std::string filepath = "Resources/Texture/" + texName;
	wchar_t wfilepath[128];

	// ユニコード文字列に変換
	MultiByteToWideChar(CP_ACP, 0, filepath.c_str(), -1, wfilepath, _countof(wfilepath));

	result = LoadFromWICFile(
		wfilepath,
		WIC_FLAGS_NONE,
		&metadata, scratchlmg);

	const Image* img = scratchlmg.GetImage(0, 0, 0);//生データ抽出	

	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	ComPtr < ID3D12Resource> texbuff;
	result = dev->CreateCommittedResource(//GPUリソースの生成
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
			D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,//テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(&texbuff));

	//データの転送
	//テクスチャバッファにデータ転送
	result = texbuff->WriteToSubresource(
		0,
		nullptr,//全領域へコピー
		img->pixels,//元データへのアドレス
		(UINT)img->rowPitch,//1ラインサイズ
		(UINT)img->slicePitch//全サイス
	);

	metadetes[name] = metadata;
	texbuffs[name] = texbuff;

	//シェーダリソースビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	srvDesc.Format = metadata.format;//RGBA
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;


	dev->CreateShaderResourceView(texbuffs[name].Get(),//ビューと関連付けるバッファ
		&srvDesc,//テクスチャ設定情報
		CD3DX12_CPU_DESCRIPTOR_HANDLE(basicDescHeape->GetCPUDescriptorHandleForHeapStart(),
			texIndex,dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	texIndexes[name] = texIndex;

	texIndex++;

}


void Texture::CreateTexture(const std::string& name, const int& width, const int& height, XMFLOAT4* color)
{
	HRESULT result;
	auto dev = DirectXLib::GetInstance()->GetDevice();

	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		(UINT64)width,
		(UINT)height,
		1,
		1
	);


	ComPtr < ID3D12Resource> texbuff;
	result = dev->CreateCommittedResource(//GPUリソースの生成
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
			D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,//テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(&texbuff));

	result = texbuff->WriteToSubresource(
		0,
		nullptr,//全領域へコピー
		color,//元データへのアドレス
		sizeof(XMFLOAT4) * width,//1ラインサイズ
		sizeof(XMFLOAT4) * width * height//全サイス
	);

	texbuffs[name] = texbuff;
	createTexSize[name].x = width;
	createTexSize[name].y = height;


	//シェーダリソースビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//RGBA
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;


	dev->CreateShaderResourceView(texbuffs[name].Get(),//ビューと関連付けるバッファ
		&srvDesc,//テクスチャ設定情報
		CD3DX12_CPU_DESCRIPTOR_HANDLE(basicDescHeape->GetCPUDescriptorHandleForHeapStart(),
			texIndex, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	texIndexes[name] = texIndex;

	texIndex++;
}

void Texture::AddTexture(const std::string& name, ID3D12Resource* texBuff)
{
	if (texbuffs[name] != nullptr)
		assert(0);
	auto dev = DirectXLib::GetInstance()->GetDevice();

	texbuffs[name] = texBuff;
	
	//シェーダリソースビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	srvDesc.Format = texBuff->GetDesc().Format;//RGBA
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;


	dev->CreateShaderResourceView(texbuffs[name].Get(),//ビューと関連付けるバッファ
		&srvDesc,//テクスチャ設定情報
		CD3DX12_CPU_DESCRIPTOR_HANDLE(basicDescHeape->GetCPUDescriptorHandleForHeapStart(),
			texIndex, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	texIndexes[name] = texIndex;

	texIndex++;

}

void Texture::SendColor(const std::string& name, XMFLOAT4* color)
{
	HRESULT result;
	result = texbuffs[name]->WriteToSubresource(
		0,
		nullptr,//全領域へコピー
		color,//元データへのアドレス
		sizeof(XMFLOAT4) * texbuffs[name].Get()->GetDesc().Width,
		sizeof(XMFLOAT4) * texbuffs[name].Get()->GetDesc().Width* texbuffs[name].Get()->GetDesc().Height
		//sizeof(XMFLOAT4) * createTexSize[name].x,//1ラインサイズ
		//sizeof(XMFLOAT4) * createTexSize[name].x * createTexSize[name].y//全サイス
	);
}

void Texture::ChangeTexture(const std::string& name, ID3D12Resource* texBuff)
{
	if (texbuffs[name] == nullptr)
		assert(0);
	auto dev = DirectXLib::GetInstance()->GetDevice();

	texbuffs[name] = texBuff;

	//シェーダリソースビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	srvDesc.Format = texBuff->GetDesc().Format;//RGBA
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;


	dev->CreateShaderResourceView(texbuffs[name].Get(),//ビューと関連付けるバッファ
		&srvDesc,//テクスチャ設定情報
		CD3DX12_CPU_DESCRIPTOR_HANDLE(basicDescHeape->GetCPUDescriptorHandleForHeapStart(),
			texIndexes[name], dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
}

void Texture::CreateChangeTex(const std::string& name, const int& width, const int& height, XMFLOAT4* color)
{
	if (texbuffs[name] == nullptr)
		assert(0);

	HRESULT result;
	auto dev = DirectXLib::GetInstance()->GetDevice();

	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		(UINT64)width,
		(UINT)height,
		1,
		1
	);


	ComPtr < ID3D12Resource> texbuff;
	result = dev->CreateCommittedResource(//GPUリソースの生成
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
			D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,//テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(&texbuff));

	result = texbuff->WriteToSubresource(
		0,
		nullptr,//全領域へコピー
		color,//元データへのアドレス
		sizeof(XMFLOAT4) * width,//1ラインサイズ
		sizeof(XMFLOAT4) * width * height//全サイス
	);

	texbuffs[name] = texbuff;
	createTexSize[name].x = width;
	createTexSize[name].y = height;


	//シェーダリソースビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	srvDesc.Format = texbuff->GetDesc().Format;//RGBA
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;


	dev->CreateShaderResourceView(texbuffs[name].Get(),//ビューと関連付けるバッファ
		&srvDesc,//テクスチャ設定情報
		CD3DX12_CPU_DESCRIPTOR_HANDLE(basicDescHeape->GetCPUDescriptorHandleForHeapStart(),
			texIndexes[name], dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
}


void Texture::Initialize()
{
	HRESULT result;
	//設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;  //シェーダーから見える
	descHeapDesc.NumDescriptors = 255;
	//生成
	result = DirectXLib::GetInstance()->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeape));
	assert(SUCCEEDED(result));
}

