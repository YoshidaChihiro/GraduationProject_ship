#include "Material.h"
#include"DirectXLib.h"
#include"Texture.h"

Material::Material()
{
}


Material::~Material()
{
}

Material * Material::Create()
{
	Material* instace = new Material;
	instace->Initialize();
	return instace;
}

void Material::Initialize()
{
	HRESULT result;

	ambient = { 0.3f, 0.3f, 0.3f };
	diffuse = { 0.0f, 0.0f, 0.0f };
	specular = { 0.0f, 0.0f, 0.0f };
	alpha = 1.0f;
	auto dev = DirectXLib::GetInstance()->GetDevice();

	// 定数バッファの生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff)&~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	if (FAILED(result)) {
		assert(0);
	}
}

void Material::LoadTexture()
{
	//テクスチャ無しの場合はテクスチャ無し用の白い画像を適用
	if (textureFileName.size() == 0)
		textureFileName = "noTex.png";
	//既にテクスチャが読み込まれている場合は即終了
	if (Texture::GetTexBuff(textureFileName) != nullptr)
		return;

	HRESULT result = S_FALSE;

	Texture::LoadTexture(textureFileName, textureFileName);	
}

void Material::Update()
{
	HRESULT result;
	// 定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->ambient = ambient;
		constMap->diffuse = diffuse;
		constMap->specular = specular;
		constMap->alpha = alpha;
		constBuff->Unmap(0, nullptr);
	}

}
