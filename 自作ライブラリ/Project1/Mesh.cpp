#include "Mesh.h"
#include <d3dcompiler.h>
#include <cassert>
#include"DirectXLib.h"
#include "Object3D.h"
#pragma comment(lib, "d3dcompiler.lib")
using namespace DirectX;
#include"Texture.h"

void Mesh::CreateBuffers()
{
	HRESULT result;
	auto dev = DirectXLib::GetInstance()->GetDevice();

	UINT sizeVB = static_cast<UINT>(sizeof(VERTEXSTATUS)*vertices.size());
	// 頂点バッファ生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	// 頂点バッファへのデータ転送
	VERTEXSTATUS* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

	if (FAILED(result)) {
		assert(0);
		return;
	}

	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short)*indices.size());
	// インデックスバッファ生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	if (FAILED(result)) {
		assert(0);
		return;
	}

	// インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {
		std::copy(indices.begin(), indices.end(), indexMap);
		indexBuff->Unmap(0, nullptr);
	}

	// インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

}

void Mesh::Draw()
{
	auto cmd = DirectXLib::GetInstance()->GetCommandList();
	cmd->IASetVertexBuffers(0, 1, &vbView);
	// インデックスバッファをセット
	cmd->IASetIndexBuffer(&ibView);


	ID3D12DescriptorHeap* ppHeaps[] = { Texture::GetBasicDescHeap().Get() };
	cmd->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);


	if (!Object3D::GetDrawShadow())
	{
		// シェーダリソースビューをセット
		cmd->SetGraphicsRootDescriptorTable(2, Texture::GetGpuDescHandleSRV(material->textureFileName));
		// マテリアルの定数バッファをセット
		ID3D12Resource* constBuff = material->GetConstBuffer();
		cmd->SetGraphicsRootConstantBufferView(1, constBuff->GetGPUVirtualAddress());
	}
	// 描画コマンド
	cmd->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);
}


void Mesh::CalculateSmoothedVertexNormals()
{
	auto itr = smoothData.begin();
	for (; itr != smoothData.end(); ++itr)
	{
		//各面用の共通頂点コレクション
		std::vector<unsigned short>& v = itr->second;
		//全頂点の法線を平均する
		XMVECTOR normal = {};
		for (unsigned short index : v)
		{
			normal += XMVectorSet(vertices[index].normal.x, vertices[index].normal.y, vertices[index].normal.z, 0);
		}
		normal = XMVector3Normalize(normal / (float)v.size());
		//共通法線を使用するすべての頂点データに書き込む
		for (unsigned short index : v)
		{
			vertices[index].normal = { normal.m128_f32[0],normal.m128_f32[1],normal.m128_f32[2] };
		}
	}

}
