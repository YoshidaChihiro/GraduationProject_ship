#include "ModeSelect.h"
#include "PtrDelete.h"
#include "Input.h"

ModeSelect::ModeSelect()
{
	next = Play;

	camera = std::make_unique<InGameCamera>();

	lightGroup.reset(LightGroup::Create());
	lightGroup->SetDirLightActive(0, true);
	lightGroup->SetDirLightColor(0, { 1,1,1 });
	lightGroup->SetDirLightDir(0, { 0.0f,-1.0f,0.2f,1 });

	for (int i = 0; i < mode_numMax; i++)
	{
		panel_sprites[i] = new Sprite();
	}
}

ModeSelect::~ModeSelect()
{
	for (int i = 0; i < mode_numMax; i++)
	{
		PtrDelete(panel_sprites[i]);
	}
}

void ModeSelect::Initialize()
{
	isEnd = false;
	next = Play;

	camera->Initialize();
	camera->SetDistance(130.0f);
	camera->SetPhi(DirectX::XMConvertToRadians(-90));
	camera->SetTheta(DirectX::XMConvertToRadians(0));

	mode = ModeSelect::NormalRace;

	Mode_sprites();
}

void ModeSelect::Update()
{
	//シーン切り替え
	if (Input::TriggerPadButton(XINPUT_GAMEPAD_A) || Input::TriggerKey(DIK_SPACE))
	{
		//どのモードのシーンに切り替えるか
		Chack_nextScene();
		ShutDown();
		return;
	}

	//選択
	Mode_mode();
	Mode_sprites();


	camera->Update();
	lightGroup->Update();
}

void ModeSelect::PreDraw()
{
	PipelineState::SetPipeline("BasicObj");

	const std::string texNames[] = {
		"gamemode_normal",
		"gamemode_time",
		"gamemode_obstacle",
		"gamemode_collect",
	};
	const Vector2 positions[] = {
		{480, 360},
		{1440, 360},
		{480, 720},
		{1440, 720},
	};
	for (int i = 0; i < mode_numMax; i++)
	{
		panel_sprites[i]->DrawSprite(texNames[i], positions[i], 0.0f, panel_scales[i]);
	}
}

void ModeSelect::PostDraw()
{
}

void ModeSelect::Mode_mode()
{
	if (Input::TriggerKey(DIK_UP) && (int)mode > 0)
	{
		mode = (GAMEMODE)((int)mode - 1);
	}
	else if (Input::TriggerKey(DIK_DOWN) && (int)mode < (mode_numMax - 1))
	{
		mode = (GAMEMODE)((int)mode + 1);
	}
}

void ModeSelect::Mode_sprites()
{
	//大きさ
	const Vector2 smallsize = {0.7f, 0.7f};
	const Vector2 bigsize = {1.0f, 1.0f};
	for (int i = 0; i < mode_numMax; i++)
	{
		panel_scales[i] = smallsize;
	}
	panel_scales[(int)mode] = bigsize;
}

void ModeSelect::Chack_nextScene()
{
	switch (mode)
	{
	case ModeSelect::NormalRace:
		next = Play;
		break;
	case ModeSelect::TimeAttack:
		next = Play;
		break;
	case ModeSelect::DbstacleRace:
		next = Play;
		break;
	case ModeSelect::ItemCollect:
		next = Play;
		break;
	default:
		next = Play;
		break;
	}
}
