#include "AreaEffect.h"
#include "OBJLoader.h"
std::vector<AreaEffect*> AreaEffect::effects = {};
AreaEffect::AreaEffect(const Vector3& pos, const XMFLOAT4& arg_color)
{
	//エフェクト部分の作成
	Create(OBJLoader::GetModel("Area"));

	position = pos;
	scale = { 3,10,3 };
	color = arg_color;
	HRESULT result;
	result = DirectXLib::GetInstance()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBuffDeta) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));
	offsetTime = 0;
	object->Update();
}

void AreaEffect::Initialize()
{
}

void AreaEffect::Update()
{
	//エフェクトのUVスクロールの更新
	offsetTime += 0.01f;
	if (offsetTime > 1.0f)
		offsetTime = 0;

	Object::Update();

}

void AreaEffect::Draw()
{
	Update();
	effects.push_back(this);
}

void AreaEffect::AreaEffectsDraw()
{
	if (effects.empty())
		return;
	//パイプラインを設定
	PipelineState::SetPipeline("AreaEffect", ADD);
	for (auto ptr = effects.begin();ptr != effects.end();ptr++)
	{
		(*ptr)->EffectDraw();
	}
	effects.clear();
}

void AreaEffect::EffectDraw()
{
	//エフェクトのUVスクロール用定数バッファの更新
	ConstBuffDeta* constMap = nullptr;
	auto result = constBuff->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));
	constMap->offsetTime.y = offsetTime;
	constBuff->Unmap(0, nullptr);

	//定数バッファをセット
	DirectXLib::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(3, constBuff->GetGPUVirtualAddress());
	//ライティング無し加算ブレンドで描画
	CustomDraw(false, false, ADD, true);

}
