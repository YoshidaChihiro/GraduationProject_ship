#include "Title.h"
#include "Audio.h"
#include "Input.h"
#include "OBJLoader.h"
#include "Object3D.h"
#include "Sprite3D.h"
#include "PtrDelete.h"

Title::Title()
{
	next = Play;

	camera = std::make_unique<InGameCamera>();
	Object3D::SetCamera(camera.get());
	Sprite3D::SetCamera(camera.get());

	lightGroup.reset(LightGroup::Create());
	Object3D::SetLightGroup(lightGroup.get());
	lightGroup->SetDirLightActive(0, true);
	lightGroup->SetDirLightColor(0, { 1,1,1 });

	logo = new Sprite();
}


Title::~Title()
{
	PtrDelete(logo);
}

void Title::Initialize()
{
	isEnd = false;
	next = Play;

	Object3D::SetCamera(camera.get());
	Sprite3D::SetCamera(camera.get());
	Object3D::SetLightGroup(lightGroup.get());
}

void Title::Update()
{
	//�V�[���؂�ւ�
	if (Input::TriggerPadButton(XINPUT_GAMEPAD_A) || Input::TriggerKey(DIK_SPACE))
	{
		Audio::AllStopSE();
		ShutDown();
		return;
	}

	//
	camera->Update();
	lightGroup->SetAmbientColor({ 1,1,1 });
	lightGroup->SetDirLightDir(0, { 0.0f,-1.0f,0.5f,1 });
	lightGroup->Update();
}

void Title::PreDraw()
{
	PipelineState::SetPipeline("Sprite");

	XMFLOAT2 pos_logo = { 1920 / 2, 1080 / 2 };
	logo->DrawSprite("title_logo", pos_logo);
}

void Title::PostDraw()
{
}
