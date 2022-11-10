#include "ResultInGame.h"
#include "PtrDelete.h"
#include "Input.h"
#include "Audio.h"
#include "Arudino.h"

ResultInGame::ResultInGame()
{
	base = new Sprite();
	goal = new Sprite();
	frame = new Sprite();
	select_mode = new Sprite();
	select_retry = new Sprite();
	select_title = new Sprite();
}

ResultInGame::~ResultInGame()
{
	PtrDelete(base);
	PtrDelete(goal);
	PtrDelete(frame);
	PtrDelete(select_mode);
	PtrDelete(select_retry);
	PtrDelete(select_title);
}

void ResultInGame::Initialize()
{
	isActive = false;
	alpha_base = 0.0f;
	isAlphaEnd_base = false;
	select = 1;
	position_frame = position_retry;
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
	else
	{
		isAlphaEnd_base = true;
	}

	if (!isAlphaEnd_base)
	{
		return;
	}

	//選択変更
	if (Input::TriggerKey(DIK_RIGHT) && select < 2)
	{
		Audio::PlaySE("SE_select", Audio::volume_se * 0.7f);
		select++;
	}
	if (Input::TriggerKey(DIK_LEFT) && select > 1)
	{
		Audio::PlaySE("SE_select", Audio::volume_se * 0.7f);
		select--;
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
	if (Input::TriggerKey(DIK_SPACE) || Arudino::GetData_microSwitch_Trigger())
	{
		isActive = false;
		Audio::PlaySE("SE_decision", Audio::volume_se * 1.0f);
		switch (select)
		{
		case 0:
			isMode = true;
			isActive = true;
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
	}
}

void ResultInGame::PostDraw()
{
	if (!isActive)
	{
		return;
	}

	PipelineState::SetPipeline("Sprite");

	if (isAlphaEnd_base)
	{
		//select_mode->DrawSprite("result_mode", position_mode);
		select_retry->DrawSprite("result_retry", position_retry);
		select_title->DrawSprite("result_title", position_title);

		frame->DrawSprite("result_frame", position_frame);
	}

	const XMFLOAT2 screenSize = { 1920,1080 };

	goal->DrawSprite("result_goal", { screenSize.x / 2,300 });
	base->DrawSprite("white1x1", { screenSize.x / 2,screenSize.y / 2 }, 0.0f, screenSize, { 0.0f,0.0f,0.0f,alpha_base }, { 0.5f,0.5f }, "NoAlphaToCoverageSprite");
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
