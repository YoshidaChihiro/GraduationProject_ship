#include "TimerRecord.h"
#include "PtrDelete.h"
#include "Easing.h"
#include "Audio.h"

TimerRecord::TimerRecord()
{
	timer = new Timer(limit_default);
	sprite_countDown = new NumberSprite(seconds_countDown);
	sprite_seconds = new NumberSprite(seconds);
	sprite_minutes = new NumberSprite(minutes);
	sprite_decimal = new NumberSprite(decimal);
	for (int i = 0; i < arrayCount_comma; i++)
	{
		sprite_comma[i] = new Sprite();
	}
	sprite_base = new Sprite();
}

TimerRecord::~TimerRecord()
{
	PtrDelete(timer);
	PtrDelete(sprite_countDown);
	PtrDelete(sprite_seconds);
	PtrDelete(sprite_minutes);
	PtrDelete(sprite_decimal);
	for (int i = 0; i < arrayCount_comma; i++)
	{
		PtrDelete(sprite_comma[i]);
	}
	PtrDelete(sprite_base);
}

void TimerRecord::Initialize()
{
	isAction = false;
	timer->Initialize();
	seconds_countDown = 0.0f;
	seconds = 0.0f;
	minutes = 0.0f;
	decimal = 0.0f;
	position_base = { 960, 64 };
	isCountDown = false;
	time_prev = 0;
	isResult = false;
	count_movePos = 0;
}

void TimerRecord::Update()
{
	SettingPosition();

	//通常
	if (isAction)
	{
		timer->Update();
		const int time_origin = timer->GetRealTime(TimerPerformance::Up);
		seconds = time_origin % 60;
		minutes = time_origin / 60;
		decimal = 0;//小数以下の処理不明
	}
	//カウントダウン
	else if (isCountDown)
	{
		timer->Update();
		seconds_countDown = timer->GetRealTime(TimerPerformance::Down) + 1;

		if (timer->IsTime())
		{
			Audio::PlaySE("SE_countdown_2", Audio::volume_se * 0.5f);
			isCountDown = false;
			isAction = true;
			timer->Initialize();
			timer->SetLimit(limit_default);
		}
		else if (time_prev != (int)seconds_countDown)
		{
			Audio::PlaySE("SE_countdown_1", Audio::volume_se * 0.5f);
		}

		time_prev = seconds_countDown;
	}
	//リザルト表示
	else if (isResult)
	{
		const int limit_movePos = 40;
		if (count_movePos < limit_movePos)
		{
			count_movePos++;
		}
		position_base.y = Easing::EaseOutCirc(64, 580, limit_movePos, count_movePos);
	}
}

void TimerRecord::Draw()
{
	PipelineState::SetPipeline("Sprite");

	if (isCountDown)
	{
		sprite_countDown->Draw(1, "number_default", position_countDown);
	}


	int digit = 2;
	sprite_seconds->Draw(digit, "number_default", position_seconds);
	sprite_minutes->Draw(digit, "number_default", position_minutes);
	//sprite_decimal->Draw(digit, "number_default", position_decimal);
	for (int i = 0; i < arrayCount_comma - 1; i++)
	{
		sprite_comma[i]->DrawSprite("number_comma", position_comma[i]);
	}
	sprite_base->DrawSprite("timer_base", position_base);
}

void TimerRecord::Start()
{
	isCountDown = true;
	isAction = false;
	isResult = false;
	timer->Initialize();
	timer->SetLimit(limit_countDown);
}

void TimerRecord::Goal()
{
	isCountDown = false;
	isAction = false;
	isResult = true;
}

bool TimerRecord::GetIsAction()
{
	return isAction;
}

void TimerRecord::SettingPosition()
{
	const XMFLOAT2 position_center = { 1920 / 2, 1080 / 2 };
	const float size_numberX = 47;

	//カウントダウン
	position_countDown = position_center;

	//ベース
	//position_base = { position_center.x, 64 };

	//秒
	position_seconds = {
		position_base.x + (size_numberX * 1.5f),
		position_base.y
	};

	//分
	position_minutes = {
		position_base.x - (size_numberX * 1.5f),
		position_base.y
	};

	//小数
	position_decimal = {
		position_base.x + (size_numberX * 3),
		position_base.y
	};

	//
	position_comma[0] = position_base;
	position_comma[1] = { position_base.x + (size_numberX * 1.5f), position_base.y };

}
