#include "Title.h"
#include "Audio.h"
#include "Input.h"
#include "OBJLoader.h"
#include "Object3D.h"
#include "Sprite3D.h"
#include "PtrDelete.h"
#include "Arudino.h"

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

	back = new Sprite();
	logo = new Sprite();
	startConfig = new Sprite();
}


Title::~Title()
{
	PtrDelete(back);
	PtrDelete(logo);
	PtrDelete(startConfig);
}

void Title::Initialize()
{
	isEnd = false;
	next = Play;

	camera->Initialize();
	Object3D::SetCamera(camera.get());
	Sprite3D::SetCamera(camera.get());
	Object3D::SetLightGroup(lightGroup.get());

	alphaStart = 1.0f;

	sceneChangeStart = false;
}

void Title::Update()
{
	//ÉVÅ[ÉìêÿÇËë÷Ç¶
	if (Input::TriggerKey(DIK_SPACE) || Arudino::GetData_microSwitch_Trigger())
	{
		Audio::PlaySE("SE_decision", Audio::volume_se * 1.0f);
		sceneChangeStart = true;
	}
	if (sceneChangeStart)
	{
		if (alphaStart <= 0.0f)
		{
			Audio::AllStopSE();
			ShutDown();
			return;
		}
		else
		{
			const float speed_subAlpha = 0.05f;
			alphaStart -= speed_subAlpha;
		}
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

	XMFLOAT2 pos_center = { 1920 / 2, 1080 / 2 };
	logo->DrawSprite("title_logo", pos_center, 0.0f, {0.9f,0.9f});

	back->DrawSprite("title_back", pos_center);

}

void Title::PostDraw()
{
	XMFLOAT2 pos_start = { 1920 / 2, 880 };
	startConfig->DrawSprite("title_start", pos_start, 0.0f, { 0.9f,0.9f }, { 1,1,1,alphaStart }, {0.5f,0.5f}, "NoAlphaToCoverageSprite");
}
