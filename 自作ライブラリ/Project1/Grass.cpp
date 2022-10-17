#include "Grass.h"

#include "DrawMode.h"
#include"OBJLoader.h"
#include "Texture.h"

Grass::Grass()
{
	Initialize(OBJLoader::GetModel("ground"));
	//Create(OBJLoader::GetModel("ground"));
	position = {0,0,0};
	CreateConstBuff();
	GrassWidth = 0;
	GrassHeight = 0;
	airDir[0] = 1.0f;
	airDir[1] = 0.0f;
	airPower = 1.0f;
	time = 0;
}

void Grass::Draw()
{
	auto cmdList = DirectXLib::GetInstance()->GetCommandList();
#ifdef _DEBUG

	if (!Object3D::GetDrawShadow()&&DrawMode::GetDrawImGui())
	{
		ImGui::Begin("Status");
		ImGui::SliderFloat("GrassHeight", &GrassHeight, 0, 20.0f);
		ImGui::SliderFloat("GrassWidth", &GrassWidth, 0, 5.0f);
		ImGui::SliderFloat2("airDir", airDir, -1.0f, 1.0f);
		ImGui::SliderFloat("airPower", &airPower, -10.0, 10.0f);

		//ImGui::SliderFloat("positionFactor", &positionFactor, 0, 2.0f);
		//ImGui::SliderFloat("rotationFactor", &rotationFactor, 0, 30.0f);

		ImGui::End();
	}
#endif
	
	time++;
	//// 定数バッファへデータ転送
	ConstBuffData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));
	constMap->playerPos = {0,0,0};
	constMap->GrassBottomColor = {0,0.2f,0,1};
	constMap->GrassTopColor = {0,1,0,1};
	constMap->playerRadius = 0;
	constMap->GrassHeight = GrassHeight;
	constMap->GrassWidth = GrassWidth;
	constMap->AirDir = Vector2::Normalize(Vector2(airDir[0], airDir[1]));
	constMap->AirPower = airPower;
	constMap->Time = time;

	constBuff->Unmap(0, nullptr);

	//floor->Draw();
				// パイプラインステートの設定
	if (Object3D::GetDrawShadow())
	{
		PipelineState::SetPipeline("GrassShadowMap");
		cmdList->SetGraphicsRootConstantBufferView(1, constBuff->GetGPUVirtualAddress());

	}
	else
	{
		PipelineState::SetPipeline("Grass");
		cmdList->SetGraphicsRootConstantBufferView(4, constBuff->GetGPUVirtualAddress());
	}
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CustomDraw( false, true, ALPHA, true);
	if (Object3D::GetDrawShadow())
		return;
	ConstLightCameraBuff* constMap2 = nullptr;

	result = constCameraBuff->Map(0, nullptr, (void**)&constMap2);
	assert(SUCCEEDED(result));
	constMap2->cameraPos = Object3D::GetLightCamera()->GetEye();
	constMap2->viewProjection = Object3D::GetLightCamera()->GetMatViewProjection();

	constCameraBuff->Unmap(0, nullptr);

	PipelineState::SetPipeline("DrawShadowOBJ");
	cmdList->SetGraphicsRootConstantBufferView(3, constCameraBuff->GetGPUVirtualAddress());
	cmdList->SetGraphicsRootDescriptorTable(4, Texture::GetGpuDescHandleSRV("shadowMap" + std::to_string(bbIndex)));  //ヒープの先頭が定数バッファ

	CustomDraw(false, false,ALPHA,true);
	
}

void Grass::CreateConstBuff()
{
	HRESULT result = DirectXLib::GetInstance()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBuffData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));
	result = DirectXLib::GetInstance()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstLightCameraBuff) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constCameraBuff));
	assert(SUCCEEDED(result));

}
