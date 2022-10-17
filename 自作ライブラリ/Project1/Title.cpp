#include "Title.h"

#include "DebugCamera.h"
#include "Sprite.h"
#include "TextureResource.h"
#include "OBJLoader.h"
#include "Audio.h"
#include "Easing.h"
#include <random>

Title::Title()
{
	next = Play;

	camera = std::make_unique<InGameCamera>();
	Object3D::SetCamera(camera.get());
	Sprite3D::SetCamera(camera.get());

	//ライト生成
	lightGroup.reset(LightGroup::Create());
	//3Dオブジェクトにライトをセット
	Object3D::SetLightGroup(lightGroup.get());
	//ライト色を設定
	lightGroup->SetDirLightActive(0, true);
	lightGroup->SetDirLightColor(0, { 1,1,1 });
}


Title::~Title()
{
}

void Title::Initialize()
{
	isEnd = false;

	cameraDistance = cameraDistance_init;
	camera.get()->SetDistance(cameraDistance);
	Object3D::SetCamera(camera.get());
	Sprite3D::SetCamera(camera.get());
	Object3D::SetLightGroup(lightGroup.get());
	easingTimer_zoom = 0;
	isSceneChange = false;

}

void Title::Update()
{
	//シーン遷移
	if (isSceneChange)
	{
			if(ZoomIn())
			{
				//シーン切り替え
				ShutDown();
			}
	}
	else
	{
		//シーン切り替え開始
		if (Input::TriggerPadButton(XINPUT_GAMEPAD_A) || Input::TriggerKey(DIK_SPACE))
		{
			isSceneChange = true;
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
	PipelineState::SetPipeline("BasicObj");

}

void Title::PostDraw()
{
}

float Title::GetRandom(float arg_min, float arg_max)
{
	std::random_device rnd;
	std::mt19937_64 mt64(rnd());
	std::uniform_real_distribution<float> genRandFloat(arg_min, arg_max);
	return genRandFloat(mt64);
}


bool Title::ZoomIn()
{
	//近づける
	const float endDistance = 7.0f;
	const int easingLimit = 45;
	cameraDistance = Easing::EaseInCirc(cameraDistance_init, endDistance, easingLimit, easingTimer_zoom);
	easingTimer_zoom++;
	//反映
	camera.get()->SetDistance(cameraDistance);

	//目標地点まで近づいたら真を返す
	return cameraDistance <= endDistance;
}

