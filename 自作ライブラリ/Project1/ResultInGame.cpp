#include "ResultInGame.h"
#include "PtrDelete.h"
#include "Input.h"

ResultInGame::ResultInGame()
{
	base = new Sprite();
	frame = new Sprite();
	select_mode = new Sprite();
	select_retry = new Sprite();
	select_title = new Sprite();
}

ResultInGame::~ResultInGame()
{
	PtrDelete(base);
	PtrDelete(frame);
	PtrDelete(select_mode);
	PtrDelete(select_retry);
	PtrDelete(select_title);
}

void ResultInGame::Initialize()
{
	isActive = false;
	alpha_base = 0.0f;
	select = 0;
	position_frame = position_mode;
	isMode = false;
	isRetry = false;
	isTitle = false;
}

void ResultInGame::Update()
{
	if (!isActive)
	{
		return;
	}

	//表示開始時に徐々に濃くなる
	const float limit_alpha = 0.5f;
	if (limit_alpha > alpha_base)
	{
		const float speed_alpha = 0.01f;
		alpha_base += speed_alpha;
	}

	//選択変更
	if (Input::DownKey(DIK_RIGHT) && select > 0)
	{
		select--;
	}
	if (Input::DownKey(DIK_LEFT) && select < 2)
	{
		select++;
	}
	switch (select)
	{
	case 0:
		position_frame = position_mode;
		break;
	case 1:
		position_frame = position_retry;
		break;
	case 2:
		position_frame = position_title;
		break;
	default:
		break;
	}

	//決定
	if (Input::TriggerPadButton(XINPUT_GAMEPAD_A) || Input::TriggerKey(DIK_SPACE))
	{
		switch (select)
		{
		case 0:
			isMode = true;
			break;
		case 1:
			isRetry = true;
			break;
		case 2:
			isTitle = true;
			break;
		default:
			break;
		}
		isActive = false;
	}
}

void ResultInGame::Draw()
{
	if (!isActive)
	{
		return;
	}

	PipelineState::SetPipeline("Sprite");

	const XMFLOAT2 screenSize = { 1920,1080 };
	base->DrawSprite("white1x1", { screenSize.x / 2,screenSize.y / 2}, 0.0f, screenSize, {0.0f,0.0f,0.0f,alpha_base }, {0.5f,0.5f}, "NoAlphaToCoverageSprite");

	frame->DrawSprite("", position_frame);

	select_mode->DrawSprite("", position_mode);
	select_retry->DrawSprite("", position_retry);
	select_title->DrawSprite("", position_title);
}

void ResultInGame::SetIsActive(const bool arg_isActive)
{
	isActive = arg_isActive;
}

bool ResultInGame::GetIsMode()
{
	return isMode;
}

bool ResultInGame::GetIsRetry()
{
	return isRetry;
}

bool ResultInGame::GetIsTitle()
{
	return isTitle;
}
