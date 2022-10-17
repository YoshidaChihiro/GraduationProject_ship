#include "CreateFigure.h"
#include<math.h>
#include<d3dx12.h>

CreateFigure::CreateFigure()
{
}


CreateFigure::~CreateFigure()
{
}

void CreateFigure::Initialize()
{
	spritePosition.x = basePos.x;
	spritePosition.y = basePos.y;
	spritePosition.z = 0.0f;
	spriteMatScale = XMMatrixScaling(baseMatScale.x, baseMatScale.y, 1);

}

void CreateFigure::Craete(float r, const int vertex, float descHeight, float descWidth, XMFLOAT3 vector3, XMFLOAT4 color)
{
	HRESULT result;
	ID3D12Device* dev = DirectXLib::GetInstance()->GetDevice();
	this->r = r;
	this->vertex = vertex;
	CreateVartex(r, vertex, vertices2, indices, descHeight, descWidth);
	for (int i = 0; i < vertex + 1; i++)
	{
		vertices2[i].x += vector3.x;
		vertices2[i].y += vector3.y;
		vertices2[i].z += vector3.z;

	}
	vertMap = nullptr;
	vertBuff = nullptr;
	indexBuff = nullptr;
	basicDescHeap = nullptr;
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用

	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = sizeof(vertices2); //頂点情報が入る分のサイズ
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	result = dev->CreateCommittedResource(
		&heapprop, //ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resdesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices2);
	vbView.StrideInBytes = sizeof(vertices2[0]);



	//設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC cbvDescHeapDesc{};
	cbvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvDescHeapDesc.NumDescriptors = 1;  //バッファは一つ
	cbvDescHeapDesc.Flags =
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;  //シェーダーから見える
	//生成
	result = dev->CreateDescriptorHeap(&cbvDescHeapDesc, IID_PPV_ARGS(&basicDescHeap));
	D3D12_HEAP_PROPERTIES cbheapprop{};   //ヒープ設定
	cbheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;    //GPUへの転送用

	D3D12_RESOURCE_DESC cbresdesc{};		//リソース設定
	cbresdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbresdesc.Width = (sizeof(ConstBufferDate) + 0xff)&~0xff; //256バイトアラインメント
	cbresdesc.Height = 1;
	cbresdesc.DepthOrArraySize = 1;
	cbresdesc.MipLevels = 1;
	cbresdesc.SampleDesc.Count = 1;
	cbresdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//GPUリソースの生成
	ID3D12Resource* constBuff = nullptr;
	result = dev->CreateCommittedResource(
		&cbheapprop,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbresdesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	result = constBuff->Map(0, nullptr, (void**)&constMap);  //マッピング
	constMap->color = color;			//RGBAで灰色
	constBuff->Unmap(0, nullptr);		//マッピング解除
	//定数バッファビューの作成
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuff->GetDesc().Width;
	dev->CreateConstantBufferView(
		&cbvDesc, basicDescHeap->GetCPUDescriptorHandleForHeapStart());

	resdesc.Width = sizeof(indices);//インデックス情報が入る分のサイズ
	//GPUリソースの生成
	result = dev->CreateCommittedResource(
		&heapprop,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resdesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	//GPU上のバッファに対応した仮想メモリを取得
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);


	//全インデックスに対して
	for (int i = 0; i < vertex * 3; i++)
	{
		indexMap[i] = indices[i];  //インデックスをコピー
	}
	indexBuff->Unmap(0, nullptr);
	//インデックスバッファビューの作成とセット
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(indices);

}

void CreateFigure::CreateSprite(float rotation ,XMFLOAT2 trans, XMFLOAT2 scale,XMFLOAT4 color, ID3D12Resource* texbuff, XMFLOAT2 anchorpoint )
{
	ID3D12Device * dev = DirectXLib::GetInstance()->GetDevice();
	HRESULT result;
	enum{LB,LT,RB,RT};
	D3D12_RESOURCE_DESC resDesc = texbuff->GetDesc();
	width = (float)resDesc.Width ; //画像の横幅
	height = (float)resDesc.Height ; //画像の縦幅
	baseAnchor = anchorpoint;
	spriteRotation = rotation;
	baseRotation = rotation;
	spritePosition.x = trans.x + (width/2*scale.x);
	spritePosition.y = trans.y + (height/2*scale.y);
	spritePosition.z = 0.0f;
	basePos = XMFLOAT2(spritePosition.x,spritePosition.y);
	spriteMatScale = XMMatrixScaling(scale.x, scale.y, 1);
	baseMatScale = scale;

	spriteMatProjection = XMMatrixOrthographicOffCenterLH(
		0.0f, 1280, 720, 0.0f, 0.0f, 1.0f);
	vertices[0].pos = { (0.0f - anchorpoint.x) * width ,(1.0f - anchorpoint.y)*height ,1.0f };
	vertices[1].pos = { (0.0f - anchorpoint.x) * width ,(0.0f - anchorpoint.y)*height ,1.0f };
	vertices[2].pos = { (1.0f - anchorpoint.x) * width ,(1.0f - anchorpoint.y)*height ,1.0f };
	vertices[3].pos = { (1.0f - anchorpoint.x) * width ,(0.0f - anchorpoint.y)*height ,1.0f };
	vertices[0].uv = { 0.0f,1.0f };
	vertices[1].uv = { 0.0f,0.0f };
	vertices[2].uv = { 1.0f,1.0f };
	vertices[3].uv = { 1.0f,0.0f };

	//vertices[0] = { {0.0f,  100.0f,0.0f},{0.0f,1.0f} };
	//vertices[1] = { {0.0f,    0.0f,0.0f},{0.0f,0.0f} };
	//vertices[2] = { {100.0f,100.0f,0.0f},{1.0f,1.0f} };
	//vertices[3] = { {100.0f,  0.0f,0.0f},{1.0f,0.0f} };

	//頂点バッファの生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertBuff)
	);

	//バッファへのデータ転送
	VERTEX* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	vertBuff->Unmap(0, nullptr);

	//頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);

	//頂点バッファビューの作成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDate) + 0xff)&~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff));
	D3D12_DESCRIPTOR_HEAP_DESC cbvDescHeapDesc{};
	cbvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvDescHeapDesc.NumDescriptors = 1;  //バッファは一つ
	cbvDescHeapDesc.Flags =
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;  //シェーダーから見える

	//定数バッファにデータを転送
	ConstBufferDate* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = color;
	constMap->mat = XMMatrixOrthographicOffCenterLH(
		0.0f, (float)window_width, (float)window_height, 0.0f, 0.0f, 1.0f);
	constBuff->Unmap(0, nullptr);
	//D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = this->basicHeap->GetCPUDescriptorHandleForHeapStart();


}

void CreateFigure::DrawSprite(XMFLOAT4 color,ID3D12DescriptorHeap* basicDescHeap, ID3D12PipelineState * pipelinestate, ID3D12RootSignature * rootsignature)
{
	ID3D12GraphicsCommandList * cmdList = DirectXLib::GetInstance()->GetCommandList();
	//ワールド行列の更新
	spriteMatWorld = XMMatrixIdentity();
	spriteMatWorld *= spriteMatScale;
	spriteMatWorld *= XMMatrixRotationZ(XMConvertToRadians(spriteRotation));
	spriteMatWorld *= spriteTrans = XMMatrixTranslation(spritePosition.x, spritePosition.y, spritePosition.z);
	//行列の転送
	ConstBufferDate* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->mat = spriteMatWorld * spriteMatProjection;//行列の合成
	constMap->color = color;
	constBuff->Unmap(0, nullptr);

	cmdList->SetPipelineState(pipelinestate);
	cmdList->SetGraphicsRootSignature(rootsignature);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { basicDescHeap };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
		//インデックスバッファのセットコマンド
	cmdList->SetGraphicsRootDescriptorTable(1, basicDescHeap->GetGPUDescriptorHandleForHeapStart());  //ヒープの先頭が定数バッファ

	cmdList->DrawInstanced(4, 1, 0, 0);

}

void CreateFigure::CreateVartex(float r, int n, XMFLOAT3 *vertices, unsigned short *indices, float descHeight, float descWidth)
{

	for (int i = 0; i < n + 1 ; i++)
	{
		if (i == n )
		{

			vertices[i].x = 0.0f;
			vertices[i].y = 0.0f;
			vertices[i].z = 0.0f;

		}
		else
		{
			vertices[i].x = r * sinf((XM_2PI / n) * (i + 1)) * (descHeight / descWidth);
			vertices[i].y = r * cosf((XM_2PI / n) * (i + 1));
			vertices[i].z = 0.0f;

		}
	}
	for (int i = 0; i < n; i++)
	{
		int k = i * 3;
		indices[k] = n ;
		indices[k + 1] = i;
		if (k + 2 == (n * 3) - 1)
			indices[k + 2] = 0;
		else
			indices[k + 2] = i + 1;
	}

}


void CreateFigure::Draw(ID3D12PipelineState* pipelinestate)
{
	ID3D12GraphicsCommandList * cmdList = DirectXLib::GetInstance()->GetCommandList();
	HRESULT result;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);

	//全頂点に対して
	for (int i = 0; i < vertex + 1; i++)
	{
		vertMap[i] = vertices2[i];
	}
	//マップｗｐ解除
	vertBuff->Unmap(0, nullptr);

	cmdList->SetPipelineState(pipelinestate);

	//デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { basicDescHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	cmdList->SetGraphicsRootDescriptorTable(0, basicDescHeap->GetGPUDescriptorHandleForHeapStart());  //ヒープの先頭が定数バッファ
		//インデックスバッファのセットコマンド
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);

	cmdList->DrawIndexedInstanced(vertex * 3, 1, 0, 0, 0);
}

void CreateFigure::ChangeWidth(float Width,float MaxHp,bool damege)
{
	spriteMatScale = XMMatrixScaling(Width, 1, 1);
	if (damege)
	{
	spritePosition.x -= width * (1.0f / (MaxHp * 0.2f));
	}
	else
	{
		spritePosition.x += width * (1.0f / (MaxHp * 0.2f));
	}
}

void CreateFigure::Move(XMFLOAT3 position)
{
	spritePosition.x = position.x + 640;
	spritePosition.y = position.y + 420;
	spritePosition.z = 0.0f;
}

bool CreateFigure::GetIsDead()
{
	return isDead;
}
