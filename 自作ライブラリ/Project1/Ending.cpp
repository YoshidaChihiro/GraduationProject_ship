#include "Ending.h"
#include "Audio.h"
#include "Input.h"
#include "OBJLoader.h"
#include "Object3D.h"

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
	next = Title;

	Object3D::SetCamera(camera.get());
	Object3D::SetLightGroup(lightGroup.get());
}

void Ending::Update()
{
	//ƒV[ƒ“Ø‚è‘Ö‚¦
	if (Input::TriggerKey(DIK_SPACE))
	{
		Audio::AllStopSE();
		ShutDown();
		return;
	}

	//
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
