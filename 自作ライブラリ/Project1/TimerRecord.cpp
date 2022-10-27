#include "TimerRecord.h"
#include "PtrDelete.h"

TimerRecord::TimerRecord()
{
	timer = new Timer(limit_default);
	sprite_seconds = new NumberSprite(seconds);
}

TimerRecord::~TimerRecord()
{
	PtrDelete(timer);
	PtrDelete(sprite_seconds);
}

void TimerRecord::Initialize()
{
	isAction = false;
	timer->Initialize();
	seconds = 0.0f;
	isCountDown = false;
}

void TimerRecord::Update()
{
	//通常
	if (isAction)
	{
		timer->Update();
		seconds = timer->GetRealTime(TimerPerformance::Up);
	}
	//カウントダウン
	else if (isCountDown)
	{
		timer->Update();
		seconds = timer->GetRealTime(TimerPerformance::Down) + 1;

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

	int digit_seconds = std::to_string((int)seconds).size();
	sprite_seconds->Draw(digit_seconds, "number_default", { 960.0f,64.0f });
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
