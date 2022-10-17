#include "NormalMapTestOBJ.h"

#include "Input.h"
#include "OBJLoader.h"
#include "Texture.h"

NormalMapTestOBJ::NormalMapTestOBJ()
{
	Create(OBJLoader::GetModel("box"));
	scale = { 2,2,2 };
}

void NormalMapTestOBJ::Update()
{

	//WASDEZでオブジェクトの回転
	Vector3 rot = {};
	const float rotVel = 0.01f;
	if (Input::DownKey(DIK_S))
	{
		rot.x -= rotVel;
	}
	if (Input::DownKey(DIK_W))
	{
		rot.x += rotVel;
	}
	if (Input::DownKey(DIK_A))
	{
		rot.y -= rotVel;

	}
	if (Input::DownKey(DIK_D))
	{
		rot.y += rotVel;

	}
	if (Input::DownKey(DIK_Z))
	{
		rot.z -= rotVel;

	}
	if (Input::DownKey(DIK_E))
	{
		rot.z += rotVel;

	}
	object->WorldUpdate(rot, NONE);
}

void NormalMapTestOBJ::Draw()
{
	auto cmd = DirectXLib::GetInstance()->GetCommandList();
	
	// パイプラインをセット
	PipelineState::SetPipeline("NormalMap");
	//デスクリプタをセット
	ID3D12DescriptorHeap* ppHeaps[] = { Texture::GetBasicDescHeap().Get() };
	cmd->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	// シェーダリソースビューをセット
	cmd->SetGraphicsRootDescriptorTable(4, Texture::GetGpuDescHandleSRV("StainedGlassNormal"));
	//パイプラインを外部でセットしてるときのDrawを呼ぶ
	CustomDraw(false,true,ALPHA,true);
}
