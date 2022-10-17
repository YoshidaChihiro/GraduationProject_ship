#include "Ending.h"
#include "Audio.h"
#include "Input.h"
#include "OBJLoader.h"
#include"Object3D.h"

Ending::Ending()
{
	next = Title;

	camera = std::make_unique<InGameCamera>();
	Object3D::SetCamera(camera.get());

	lightGroup.reset(LightGroup::Create());
	Object3D::SetLightGroup(lightGroup.get());
	lightGroup->SetDirLightActive(0, true);
	lightGroup->SetDirLightColor(0, { 1,1,1 });

}


Ending::~Ending()
{
}

void Ending::Initialize()
{
	isEnd = false;

	camera->Initialize();
	camera.get()->SetDistance(30);
	camera.get()->SetTheta(0.5f);
	Object3D::SetCamera(camera.get());
	Object3D::SetLightGroup(lightGroup.get());

	Audio::PlayBGM("BGM_Result", 0.1f * Audio::volume_bgm);
}

void Ending::Update()
{
	camera->Update();
	lightGroup->SetAmbientColor({ 1,1,1 });
	lightGroup->SetDirLightDir(0, { 0.0f,-1.0f,0.2f,1 });
	lightGroup->Update();
}

void Ending::PreDraw()
{
}

void Ending::PostDraw()
{
}

