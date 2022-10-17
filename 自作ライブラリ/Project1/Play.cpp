#include "Play.h"
#include <fstream>
#include <sstream>
#include"Object3D.h"
#include"Audio.h"
#include"ParticleEmitter.h"
#include"ParticleManager.h"
#include "DrawMode.h"
#include"imgui.h"
#include"TouchAbleObject.h"
#include"FBXManager.h"
#include "Input.h"
#include "MapBox.h"
#include "Easing.h"
#include "LevelEditor.h"
#include "Player.h"
#include "Ending.h"
#include "TextureResource.h"
#include "PtrDelete.h"
#include "ScreenCamera.h"
#include "InGameCamera.h"

#include"AreaEffect.h"
#include"Sprite3D.h"
Play::Play()
{
	next = Ending;
	camera = std::make_unique<InGameCamera>();
	Object3D::SetCamera(camera.get());
	Sprite3D::SetCamera(camera.get());

	
	//ParticleEmitter::Initialize(camera.get());
	ParticleManager::GetInstance()->SetCamera(camera.get());
	//Player::SetCamera(camera.get());
	//ライト生成
	lightGroup.reset(LightGroup::Create());
	//3Dオブジェクトにライトをセット
	Object3D::SetLightGroup(lightGroup.get());
	//ライト色を設定
	lightGroup->SetDirLightActive(0, true);
	lightGroup->SetDirLightColor(0, XMFLOAT3(color0));
	collisionManager = CollisionManager::GetInstance();
	objectManager = ObjectManager::GetInstance();

	objectManager->AddObjectsAtOnce();

	gameTime = 120;
	limit30Time = gameTime - 30;
	limit10Time = gameTime - 10;

	//pause = new Pause();
	//timeLimit = new TimeLimit(gameTime * 60);//制限時間の設定はここ

	ParticleEmitter::SetObjectManager(objectManager);
}


Play::~Play()
{
	LevelEditor::GetInstance()->Clear();
	//PtrDelete(pause);
	//PtrDelete(timeLimit);
	ParticleManager::GetInstance()->ClearDeadEffect();
}

void Play::Initialize()
{
	next = Ending;

	Object3D::SetCamera(camera.get());
	Object3D::SetLightGroup(lightGroup.get());

	objectManager->Reset();


	Player* player = new Player(Vector3(0, -5, -10));
	objectManager->Add(player);	
	
	camera->Initialize();

	isEnd = false;
	//pause->Initialize();
	//timeLimit->Initialize();
	gameEndCount = 0;

	//nowPlayingBGMName = "BGM_Play";
	//Audio::StopBGM(nowPlayingBGMName);
	//Audio::PlayBGM(nowPlayingBGMName, 0.1f * Audio::volume_bgm);


	Input::Update();
	Update();

	limit30trigger = false;

	countDownTime = 0;
	finishSoundTrigger = false;

	ParticleManager::GetInstance()->ClearDeadEffect();
}

void Play::Update()
{

	//pause->Update();
	//ゲームにもどる
	//if (pause->GetToGame())
	//{
	//	return;
	//}
	////やり直す
	//if (pause->GetRestart())
	//{
	//	Audio::StopBGM(nowPlayingBGMName);
	//	Initialize();
	//	return;
	//}
	//タイトルにもどる
	//if (pause->GetToTitle())
	//{
	//	Audio::StopBGM(nowPlayingBGMName);
	//	Audio::AllStopSE();
	//	next = Title;
	//	ShutDown();
	//	return;
	//}
	////ポーズ画面を開いているとき
	//if (pause->GetActivePause())
	//{
	//	//BGMの音量変更
	//	Audio::VolumeChangeBGM(nowPlayingBGMName, 0.1f * Audio::volume_bgm);
	//	return;
	//}


#ifdef _DEBUG
	if (Input::TriggerKey(DIK_E))//終了処理
	{
		Audio::StopBGM(nowPlayingBGMName);
		Audio::AllStopSE();
		ShutDown();
		return;
	}
#endif
	
	//timeLimit->Update();
	//if (timeLimit->GetLimit())
	//{
	//	//gameEndCount++;
	//	if (!finishSoundTrigger)
	//	{
	//		finishSoundTrigger = true;
	//	}
	//	gameEndCount++;
	//	return;
	//}



	lightGroup->SetAmbientColor(XMFLOAT3(coloramb));
	lightGroup->SetDirLightDir(0, { lightDir[0],lightDir[1],lightDir[2],1 });
	lightGroup->Update();
	objectManager->Update();
	camera->Update();
	collisionManager->CheckAllCollisions();

	ParticleManager::GetInstance()->UpdateDeadEffect();
}

void Play::PreDraw()
{
	//timeLimit->Draw();

	objectManager->DrawReady();
#ifdef _DEBUG
	if (DrawMode::GetDrawImGui() && !Object3D::GetDrawShadow())
	{
		ImGui::Begin("Light");
		ImGui::SliderFloat3("LightDir", lightDir, -1.0f, 1.0f);

		ImGui::End();
		Object3D::GetLightCamera()->SetLightDir({ lightDir[0],lightDir[1] ,lightDir[2] });
		LevelEditor::GetInstance()->Draw();
	}
#endif
		objectManager->PreDraw();
		AreaEffect::AreaEffectsDraw();
		ParticleManager::GetInstance()->DrawDeadEffect();
}

void Play::PostDraw()
{
	//if (migrate)
	//	return;

	objectManager->PostDraw();
	if (!Object3D::GetDrawShadow())
	{
		DirectXLib::GetInstance()->DepthClear();
	}
	//pause->Draw();
}

