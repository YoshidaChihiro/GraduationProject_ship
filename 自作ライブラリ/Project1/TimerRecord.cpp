#include "TimerRecord.h"
#include "PtrDelete.h"

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
	isCountDown = false;
}

void TimerRecord::Update()
{
	//通常
	if (isAction)
	{
		timer->Update();
		const int time_origin = timer->GetRealTime(TimerPerformance::Up);
		seconds = time_origin % 60;
		minutes = time_origin / 60;
		decimal = 0;/////////////
	}
	//カウントダウン
	else if (isCountDown)
	{
		timer->Update();
		seconds_countDown = timer->GetRealTime(TimerPerformance::Down) + 1;

		if (timer->IsTime())
		{
			isCountDown = false;
			isAction = true;
			timer->Initialize();
			timer->SetLimit(limit_default);
		}
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
	sprite_decimal->Draw(digit, "number_default", position_decimal);
	for (int i = 0; i < arrayCount_comma; i++)
	{
		sprite_comma[i]->DrawSprite("number_comma", position_comma[i]);
	}
	sprite_base->DrawSprite("timer_base",position_inGame);
}

void TimerRecord::Start()
{
	isCountDown = true;
	isAction = false;
	timer->Initialize();
	timer->SetLimit(limit_countDown);
}

void TimerRecord::Goal()
{
	isAction = false;
}

bool TimerRecord::GetIsAction()
{
	return isAction;
}
