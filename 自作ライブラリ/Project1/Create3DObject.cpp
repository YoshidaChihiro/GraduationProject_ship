#include "Create3DObject.h"
#include<math.h>
#include<d3dx12.h>


Create3DObject::Create3DObject()
{
}


Create3DObject::~Create3DObject()
{
}

void Create3DObject::CreatePyramid(const int vertex, const float topHeight, const int R, XMFLOAT3 scale, XMFLOAT3 rot, XMFLOAT3 trans, bool projective, XMFLOAT4 color, ID3D12Resource* texbuff, TexMetadata& metadata)
{
	HRESULT result;
	ID3D12Device * dev = DirectXLib::GetInstance()->GetDevice();
	baseColor = color;
	basePosition = trans;
	this->vertex = vertex;
	this->r = topHeight / 2.0f;
	for (int i = 0; i < vertex; i++)
	{
		vertices[i].pos.x = R * sinf((XM_2PI / (vertex - 1)) * i) /** ((float)window_height / (float)window_width)*/;
		vertices[i].pos.y = R * cosf((XM_2PI / (vertex - 1)) * i);
		vertices[i].pos.z = 0.0f;
	}
	vertices[vertex - 1].pos = { 0,0,0 };
	vertices[vertex].pos = { 0,0,-topHeight };
	XMFLOAT2 uv = { 0,0 };
	for (int i = 0; i < vertex + 1; i++)
	{
		uv.x = (1.0f / vertex)*i;
		uv.y = (1.0f / vertex)*i;
		vertices[i].uv = uv;
	}

	for (int i = 0; i < (vertex - 1) * 2; i++)
	{
		int k = i * 3;
		if (i < vertex - 1)
		{
			if (i == vertex - 2)
			{
				indices[k] = 0;
				indices[k + 1] = vertex - 2;
			}
			else
			{
				indices[k] = i + 1;
				indices[k + 1] = i;
			}
			indices[k + 2] = vertex - 1;
		}
		else
		{
			indices[k] = i - vertex + 1;
			if (i == (vertex - 1) * 2 - 1)
			{
				indices[k + 1] = 0;
			}
			else
			{
				indices[k + 1] = indices[k] + 1;
			}
			indices[k + 2] = vertex;
		}
	}
	for (int i = 0; i < (vertex -1)*2; i++)
	{
		//三角形1つ毎に計算していく
		//三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short index0 = indices[i * 3 + 0];
		unsigned short index1 = indices[i * 3 + 1];
		unsigned short index2 = indices[i * 3 + 2];
		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);
		//p0->p1ベクトル,p0->p2ベクトルを計算(ベクトルの減算)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//外積は両方から垂直なベクトル
		XMVECTOR normal = XMVector3Cross(v1, v2);
		//正規化(長さ1にする)
		normal = XMVector3Normalize(normal);
		//求めた法線を頂点データに代入
		XMStoreFloat3(&vertices[index0].normal, normal);
		XMStoreFloat3(&vertices[index1].normal, normal);
		XMStoreFloat3(&vertices[index2].normal, normal);
	}
	matWorld = XMMatrixIdentity();
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matWorld *= matScale;//ワールド行列にスケーリングを反映
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationY(XMConvertToRadians(rot.y));//y軸周りに45度回転
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rot.z));//Z軸周りに45度回転
	matRot *= XMMatrixRotationX(XMConvertToRadians(rot.x));//x軸周りに45度回転
	matWorld *= matRot;//ワールド行列に回転を反映
	matTrans = XMMatrixTranslation(trans.x, trans.y, trans.z); //(50,0,0)平行移動
	matWorld *= matTrans;//ワールド行列に平行移動を反映
	this->position = trans;
	this->rotation = rot;
	baseRot = rot;
	baseScale = matScale;

	D3D12_HEAP_PROPERTIES heapprop{};//頂点ヒープ設定
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用

	D3D12_RESOURCE_DESC resdesc{};  //リソース設定
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = sizeof(vertices); //頂点情報が入る分のサイズ
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//GPUリソースの生成
	result = dev->CreateCommittedResource(
		&heapprop, //ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resdesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);


	//設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;  //シェーダーから見える
	descHeapDesc.NumDescriptors = 3;  //SRV1つとCBV2つ
	 //生成
	result = dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));

	//デスクリプタヒープの先頭ハンドルを取得しておく
	D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_HEAP_PROPERTIES cbheapprop{};   //ヒープ設定
	cbheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;    //GPUへの転送用

	//リソース設定
	D3D12_RESOURCE_DESC depthResDesc{};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = window_width;//レンダーターゲットに合わせる
	depthResDesc.Height = window_height;//レンダーターゲットに合わせる
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;//深度フォーマット
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//デプスステンシル
	//深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;//深度値1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

	//リソース設定
	result = dev->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度値書き込みに使用
		&depthClearValue,
		IID_PPV_ARGS(&depthBuffer));

	//深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;//深度ビューは1つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//デプスステンシルビュー
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	result = dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
	//深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart());



	D3D12_RESOURCE_DESC cbresdesc{};		//リソース設定
	cbresdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbresdesc.Width = (sizeof(ConstBufferDate) + 0xff)&~0xff; //256バイトアラインメント
	cbresdesc.Height = 1;
	cbresdesc.DepthOrArraySize = 1;
	cbresdesc.MipLevels = 1;
	cbresdesc.SampleDesc.Count = 1;
	cbresdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//GPUリソースの生成
	result = dev->CreateCommittedResource(
		&cbheapprop,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbresdesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));


	result = constBuff->Map(0, nullptr, (void**)&constMap);  //マッピング
	constMap->color = color;		//RGBAで灰色
	//透視投影
	if (projective == true)
	{
		matProjection = XMMatrixPerspectiveFovLH(
			XMConvertToRadians(60.0f),
			(float)window_width / window_height,
			0.1f, 1000.0f
		);
	}
	else
	{
		matProjection = XMMatrixOrthographicOffCenterLH(
			-100, 100, -60, 60, 0.1f, 1000.0f
		);
	}
	constMap->mat = matWorld * camera->GetMatView() * matProjection;
	constBuff->Unmap(0, nullptr);		//マッピング解除




	//定数バッファビューの作成
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuff->GetDesc().Width;
	//dev->CreateConstantBufferView(
	//	&cbvDesc, basicDescHeap->GetCPUDescriptorHandleForHeapStart());
	dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
	//消費したぶんだけアドレスをずらす
	basicHeapHandle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//シェーダリソースビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	srvDesc.Format = metadata.format;//RGBA
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	dev->CreateShaderResourceView(texbuff,//ビューと関連付けるバッファ
		&srvDesc,//テクスチャ設定情報
		basicHeapHandle
	);

	//インデックスバッファの確保
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
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];  //インデックスをコピー
	}
	indexBuff->Unmap(0, nullptr);

	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(indices);

	//デスクリプタヒープの先頭アドレス(GPU)を記録
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleStart = basicDescHeap->GetGPUDescriptorHandleForHeapStart();
	//デスクリプタ1つ分のバッファサイズを取得
	UINT descHandleIncrementSize = dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//CBV1つ目
	gpuDescHandleCBV = gpuDescHandleStart;
	gpuDescHandleCBV.ptr += descHandleIncrementSize * 0;
	//SRV
	gpuDescHandleSRV = gpuDescHandleStart;
	gpuDescHandleSRV.ptr += descHandleIncrementSize * 1;


}

void Create3DObject::CreateBox(const int vertex, const float TopHeight, const int Width, XMFLOAT3 scale, XMFLOAT3 rot, XMFLOAT3 trans, bool projective, XMFLOAT4 color, ID3D12Resource * texbuff, TexMetadata & metadata)
{
	ID3D12Device * dev = DirectXLib::GetInstance()->GetDevice();
	HRESULT result;
	baseColor = color;
	this->vertex = vertex;
	this->width = Width;
	this->height = Width;
	this->r = (float)Width;
		int radius = (int)Width / (int)sin(XM_2PI / vertex);
		for (int i = 0; i < vertex; i++)
		{
			vertices[i].pos.x = radius * sinf((XM_2PI / vertex) * i);
			vertices[i].pos.y = radius * cosf((XM_2PI / vertex) * i);
			vertices[i].pos.z = TopHeight * 0.5f;
		}
		vertices[vertex].pos = { 0,0,TopHeight * 0.5f };
		for (int i = vertex + 1; i < (vertex + 1) * 2 - 1; i++)
		{
			vertices[i].pos.x = radius * sinf((XM_2PI / vertex) * (i - vertex - 1));
			vertices[i].pos.y = radius * cosf((XM_2PI / vertex) * (i - vertex - 1));
			vertices[i].pos.z = -TopHeight * 0.5f;
		}
		vertices[(vertex + 1) * 2 - 1].pos = { 0,0,-TopHeight * 0.5f };
		XMFLOAT2 uv = { 0,0 };
		for (int i = 0; i < (vertex + 1)*2; i++)
		{
			uv.x = (1.0f / vertex)*i;
			uv.y = (1.0f / vertex)*i;
			vertices[i].uv = uv;
		}
		int k = 0;
		for (int i = 0; i < vertex * 2; i++)
		{
			if (i <= vertex - 1)
			{
				if (i == vertex - 1)
				{
					indices[k] = 0;
					indices[k + 1] = vertex - 1;
				}
				else
				{
					indices[k] = i + 1;
					indices[k + 1] = i;
				}
				indices[k + 2] = vertex;
			}
			else if (i <= vertex * 2 - 1)
			{
				if (i == vertex * 2 - 1)
				{
					indices[k] = (vertex + 1) * 2 - 2;
					indices[k + 1] = vertex + 1;
				}
				else
				{
					indices[k] = i + 1;
					indices[k + 1] = i + 2;
				}
				indices[k + 2] = (vertex + 1) * 2 - 1;
			}
			k += 3;
		}
		for (int i = 0; i < vertex; i++)
		{
			if (i == vertex - 1)
			{
				indices[k] = 0;
				indices[k + 1] = vertex + 1;
				indices[k + 2] = vertex - 1;
				indices[k + 3] = vertex + 1;
				indices[k + 4] = (vertex + 1) * 2 - 2;
				indices[k + 5] = vertex - 1;
			}
			else
			{
				indices[k] = vertex + 1 + i;
				indices[k + 1] = i;
				indices[k + 2] = vertex + 2 + i;
				indices[k + 3] = i;
				indices[k + 4] = i + 1;
				indices[k + 5] = vertex + 2 + i;
				k += 6;
			}
		}
	//}
	for (int i = 0; i < vertex * 2 * 2; i++)
	{
		//三角形1つ毎に計算していく
		//三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short index0 = indices[i * 3 + 0];
		unsigned short index1 = indices[i * 3 + 1];
		unsigned short index2 = indices[i * 3 + 2];
		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);
		//p0->p1ベクトル,p0->p2ベクトルを計算(ベクトルの減算)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//外積は両方から垂直なベクトル
		XMVECTOR normal = XMVector3Cross(v1, v2);
		//正規化(長さ1にする)
		normal = XMVector3Normalize(normal);
		//求めた法線を頂点データに代入
		XMStoreFloat3(&vertices[index0].normal, normal);
		XMStoreFloat3(&vertices[index1].normal, normal);
		XMStoreFloat3(&vertices[index2].normal, normal);
	}
	matWorld = XMMatrixIdentity();
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matWorld *= matScale;//ワールド行列にスケーリングを反映
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationY(XMConvertToRadians(rot.y));//y軸周りに45度回転
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rot.z));//Z軸周りに45度回転
	matRot *= XMMatrixRotationX(XMConvertToRadians(rot.x));//x軸周りに45度回転
	matWorld *= matRot;//ワールド行列に回転を反映
	matTrans = XMMatrixTranslation(trans.x, trans.y, trans.z); //(50,0,0)平行移動
	matWorld *= matTrans;//ワールド行列に平行移動を反映
	this->position = trans;
	this->rotation = rot;

	D3D12_HEAP_PROPERTIES heapprop{};//頂点ヒープ設定
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用

	D3D12_RESOURCE_DESC resdesc{};  //リソース設定
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = sizeof(vertices); //頂点情報が入る分のサイズ
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//GPUリソースの生成
	result = dev->CreateCommittedResource(
		&heapprop, //ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resdesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);


	//設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;  //シェーダーから見える
	descHeapDesc.NumDescriptors = 3;  //SRV1つとCBV2つ
	 //生成
	result = dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));

	//デスクリプタヒープの先頭ハンドルを取得しておく
	D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_HEAP_PROPERTIES cbheapprop{};   //ヒープ設定
	cbheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;    //GPUへの転送用

	//リソース設定
	D3D12_RESOURCE_DESC depthResDesc{};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = window_width;//レンダーターゲットに合わせる
	depthResDesc.Height = window_height;//レンダーターゲットに合わせる
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;//深度フォーマット
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//デプスステンシル
	//深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;//深度値1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

	//リソース設定
	result = dev->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度値書き込みに使用
		&depthClearValue,
		IID_PPV_ARGS(&depthBuffer));

	//深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;//深度ビューは1つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//デプスステンシルビュー
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	result = dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
	//深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart());



	D3D12_RESOURCE_DESC cbresdesc{};		//リソース設定
	cbresdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbresdesc.Width = (sizeof(ConstBufferDate) + 0xff)&~0xff; //256バイトアラインメント
	cbresdesc.Height = 1;
	cbresdesc.DepthOrArraySize = 1;
	cbresdesc.MipLevels = 1;
	cbresdesc.SampleDesc.Count = 1;
	cbresdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//GPUリソースの生成
	result = dev->CreateCommittedResource(
		&cbheapprop,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbresdesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));


	result = constBuff->Map(0, nullptr, (void**)&constMap);  //マッピング
	constMap->color = color;		//RGBAで灰色
	//透視投影
	if (projective == true)
	{
		matProjection = XMMatrixPerspectiveFovLH(
			XMConvertToRadians(60.0f),
			(float)window_width / window_height,
			0.1f, 1000.0f
		);
	}
	else
	{
		matProjection = XMMatrixOrthographicOffCenterLH(
			-100, 100, -60, 60, 0.1f, 1000.0f
		);
	}
	constMap->mat = matWorld * camera->GetMatView() * matProjection;
	constBuff->Unmap(0, nullptr);		//マッピング解除




	//定数バッファビューの作成
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuff->GetDesc().Width;
	//dev->CreateConstantBufferView(
	//	&cbvDesc, basicDescHeap->GetCPUDescriptorHandleForHeapStart());
	dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
	//消費したぶんだけアドレスをずらす
	basicHeapHandle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//シェーダリソースビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	srvDesc.Format = metadata.format;//RGBA
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	dev->CreateShaderResourceView(texbuff,//ビューと関連付けるバッファ
		&srvDesc,//テクスチャ設定情報
		basicHeapHandle
	);

	//インデックスバッファの確保
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
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];  //インデックスをコピー
	}
	indexBuff->Unmap(0, nullptr);

	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(indices);

	//デスクリプタヒープの先頭アドレス(GPU)を記録
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleStart = basicDescHeap->GetGPUDescriptorHandleForHeapStart();
	//デスクリプタ1つ分のバッファサイズを取得
	UINT descHandleIncrementSize = dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//CBV1つ目
	gpuDescHandleCBV = gpuDescHandleStart;
	gpuDescHandleCBV.ptr += descHandleIncrementSize * 0;
	//SRV
	gpuDescHandleSRV = gpuDescHandleStart;
	gpuDescHandleSRV.ptr += descHandleIncrementSize * 1;

}

void Create3DObject::DrawPyramid( XMFLOAT4 color,ID3D12PipelineState * pipelinestate, ID3D12RootSignature * rootsignature)
{
	ID3D12GraphicsCommandList * cmdList = DirectXLib::GetInstance()->GetCommandList();
	HRESULT result;
	baseColor = color;
	if (isDamege)
	{
		damegeCounter--;
		if (damegeCounter <= 0)
		{
			damegeCounter = 20;
			isDamege = false;
		}
		constMap->color = XMFLOAT4(1.0f, 0.0f, 0.0f, color.w);
	}
	else
	{
		constMap->color = color;
	}

	result = vertBuff->Map(0, nullptr, (void**)&vertMap);

	//全頂点に対して
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];
	}
	//マップｗｐ解除
	vertBuff->Unmap(0, nullptr);
	ID3D12DescriptorHeap* ppHeaps[] = { basicDescHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	cmdList->IASetVertexBuffers(0, 1, &vbView);

	cmdList->SetPipelineState(pipelinestate);
	cmdList->SetGraphicsRootSignature(rootsignature);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->IASetIndexBuffer(&ibView);

	//定数バッファビューコマンドをセット
	cmdList->SetGraphicsRootDescriptorTable(0, gpuDescHandleCBV);
	//シェーダリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandleSRV);

	cmdList->DrawIndexedInstanced((vertex - 1) * 6, 1, 0, 0, 0);

}

void Create3DObject::DrawBox(XMFLOAT4 color,ID3D12PipelineState * pipelinestate, ID3D12RootSignature * rootsignature)
{
	ID3D12GraphicsCommandList * cmdList = DirectXLib::GetInstance()->GetCommandList();
	baseColor = color;
	HRESULT result;
	if (isDamege)
	{
		damegeCounter--;
		if (damegeCounter <= 0)
		{
			damegeCounter = 20;
			isDamege = false;
		}
		constMap->color = XMFLOAT4(1.0f, 0.0f, 0.0f, color.w);
	}
	else
	{
		constMap->color = color;
	}

	result = vertBuff->Map(0, nullptr, (void**)&vertMap);

	//全頂点に対して
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];
	}
	//マップｗｐ解除
	vertBuff->Unmap(0, nullptr);
	ID3D12DescriptorHeap* ppHeaps[] = { basicDescHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	cmdList->IASetVertexBuffers(0, 1, &vbView);

	cmdList->SetPipelineState(pipelinestate);
	cmdList->SetGraphicsRootSignature(rootsignature);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->IASetIndexBuffer(&ibView);

	//定数バッファビューコマンドをセット
	cmdList->SetGraphicsRootDescriptorTable(0, gpuDescHandleCBV);
	//シェーダリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandleSRV);

	cmdList->DrawIndexedInstanced(vertex  * 12, 1, 0, 0, 0);

}

void Create3DObject::Move()
{
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));//z軸周りに回転
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));//x軸周りに回転
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));//y軸周りに回転

	matTrans = XMMatrixTranslation(position.x, position.y, position.z);//平行移動行列を座標から再計算
	matWorld = XMMatrixIdentity();//ワールド行列は毎フレームリセット
	matWorld *= matRot;
	matWorld *= matTrans;
	constMap->mat = matWorld * camera->GetMatView() * matProjection;
}

void Create3DObject::Update()
{
}

void Create3DObject::SetDirection()
{
}



XMFLOAT3 Create3DObject::GetPosition()
{
	return position;
}

//void Create3DObject::Hit(Create3DObject * object)
//{
//	float length = sqrt(((position.x - object->position.x)*(position.x - object->position.x)) + ((position.y - r - object->position.y - object->r)*(position.y - r - object->position.y - object->r)));
//	if (length <= r + object->r)
//	{
//		object->Damege();
//		Damege();
//}

void Create3DObject::Damege()
{
	hp -= 10;
	if (hp <= 0)
	{
		isDead = true;
	}
	isDamege = true;
}

bool Create3DObject::GetIsDead()
{
	return isDead;
}

void Create3DObject::Die()
{
	isDead = true;
}

float Create3DObject::GetR()
{
	return r;
}

int Create3DObject::GetWidth()
{
	return width;
}

