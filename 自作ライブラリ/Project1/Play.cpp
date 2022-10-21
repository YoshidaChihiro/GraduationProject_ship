#include "Play.h"
#include "Object3D.h"
#include "Sprite3D.h"
#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include "Audio.h"
#include "Input.h"
#include "Player.h"

Play::Play()
{
	next = Ending;

	camera = std::make_unique<InGameCamera>();
	Object3D::SetCamera(camera.get());
	Sprite3D::SetCamera(camera.get());

	ParticleEmitter::Initialize();
	ParticleManager::GetInstance()->SetCamera(camera.get());

	lightGroup.reset(LightGroup::Create());
	Object3D::SetLightGroup(lightGroup.get());
	lightGroup->SetDirLightActive(0, true);
	lightGroup->SetDirLightColor(0, { 1,1,1 });

	collisionManager = CollisionManager::GetInstance();

	objectManager = ObjectManager::GetInstance();
	objectManager->AddObjectsAtOnce();
	ParticleEmitter::SetObjectManager(objectManager);
}


Play::~Play()
{
	ParticleManager::GetInstance()->ClearDeadEffect();
}

void Play::Initialize()
{
	isEnd = false;
	next = Ending;

	camera->Initialize();
	Object3D::SetCamera(camera.get());
	Object3D::SetLightGroup(lightGroup.get());

	objectManager->Reset();

	player = new Player(Vector3(0, 0, -10));
	objectManager->Add(player);

	ParticleManager::GetInstance()->ClearDeadEffect();
}

void Play::Update()
{
	//ƒV[ƒ“Ø‚è‘Ö‚¦
	if (Input::TriggerPadButton(XINPUT_GAMEPAD_A) || Input::TriggerKey(DIK_SPACE))
	{
		Audio::AllStopSE();
		ShutDown();
		return;
	}

	camera->AutoFocus(player->GetPosition());
	camera->Update();
	lightGroup->SetAmbientColor({ 1,1,1 });
	lightGroup->SetDirLightDir(0, { 0.0f,-1.0f,0.2f,1 });
	lightGroup->Update();
	objectManager->Update();
	collisionManager->CheckAllCollisions();

	ParticleManager::GetInstance()->UpdateDeadEffect();
}

void Play::PreDraw()
{
	objectManager->DrawReady();

	objectManager->PreDraw();
	ParticleManager::GetInstance()->DrawDeadEffect();
}

void Play::PostDraw()
{
	objectManager->PostDraw();
	if (!Object3D::GetDrawShadow())
	{
		DirectXLib::GetInstance()->DepthClear();
	}
}

