#pragma once
#include<DirectXTex.h>
#include<vector>
#include<unordered_map> 
#include<d3dx12.h>
#include<string>

#include "DirectXLib.h"
using namespace Microsoft::WRL;
using namespace DirectX;

class Texture
{
public:
	///<summary>
	///2D画像を読み込む
	///</summary>
	///<param name="name">リストのキー用の名前</param>
	///<param name="texName">読み込みたいテクスチャのファイル名</param>
	static void LoadTexture(const std::string& name, const std::string& texName);
	///<summary>
	///2D画像を自作して登録
	///</summary>
	///<param name="name">リストのキー用の名前</param>
	///<param name="width">作成する画僧の横幅</param>
	///<param name="height">作成する画僧の縦幅</param>
	///<param name="color">色配列のポインタ</param>
	static void CreateTexture(const std::string& name, const int& width, const int& height, XMFLOAT4* color);
	///<summary>
	///外部で作成したテクスチャバッファをリストに登録
	///</summary>
	///<param name="name">リストのキー用の名前</param>
	///<param name="texBuff">登録するテクスチャバッファ</param>
	static void AddTexture(const std::string& name, ID3D12Resource* texBuff);
	///<summary>
	///自作2D画像の色を変更する
	///</summary>
	///<param name="name">リストのキー用の名前</param>
	///<param name="color">色配列のポインタ</param>
	static void SendColor(const std::string& name, XMFLOAT4* color);
	static void ChangeTexture(const std::string& name, ID3D12Resource* texBuff);
	static void CreateChangeTex(const std::string& name, const int& width, const int& height, XMFLOAT4* color);
	static void Initialize();
	static const char& GetTexIndex(const std::string& name) { return texIndexes[name]; };
	inline static ComPtr < ID3D12Resource> GetTexBuff(std::string name) { return texbuffs[name]; };
	static TexMetadata& GetMetadate(std::string name) { return metadetes[name]; };
	inline static ComPtr < ID3D12DescriptorHeap > GetBasicDescHeap() {
		if (basicDescHeape == nullptr)
			assert(0);
		return basicDescHeape;
	};
	inline static CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuDescHandleSRV(const std::string& name)
	{
		if (texIndexes.find(name) == texIndexes.end())
			assert(0);
		CD3DX12_GPU_DESCRIPTOR_HANDLE descHeap = CD3DX12_GPU_DESCRIPTOR_HANDLE(basicDescHeape->GetGPUDescriptorHandleForHeapStart(),
			texIndexes[name], DirectXLib::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		return descHeap;
	};
private:
	static std::unordered_map<std::string, TexMetadata> metadetes;
	static std::unordered_map<std::string, ComPtr <ID3D12Resource> >texbuffs;
	static ComPtr < ID3D12DescriptorHeap>basicDescHeape;
	static std::unordered_map<std::string, unsigned char> texIndexes;
	static unsigned char texIndex;

	//自作画像の幅と高さを保存しておく変数
	static std::unordered_map<std::string, XMINT2> createTexSize;
};

