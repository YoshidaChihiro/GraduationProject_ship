#include "Timer.h"
#include "Easing.h"

float Timer::fps = 60.0f;

Timer::Timer()
	:Timer(600)
{
}

Timer::Timer(const unsigned int arg_limit)
	:nowCount(0),
	 limitCount(arg_limit)
{
	Initialize();
}

Timer::~Timer()
{
}

void Timer::Initialize()
{
	nowCount = 0;
}

void Timer::Update()
{
	if (IsTime())
	{
		return;
	}

	nowCount++;
}

void Timer::Reset()
{
	Initialize();
}

int Timer::GetLimit() const
{
	return limitCount;
}

void Timer::SetLimit(const unsigned int arg_limit, const bool arg_isReset)
{
	limitCount = arg_limit;
	if (arg_isReset)
	{
		Reset();
	}
}

int Timer::GetTime(const TimerPerformance arg_performance)const
{
	static int answer = 0;
	switch (arg_performance)
	{
	case TimerPerformance::Up:
			answer = nowCount;
			break;
	case TimerPerformance::Down:
		answer = limitCount - nowCount;
		break;
	default:
		answer = nowCount;
		break;
	}	
	return answer;
}

float Timer::GetRealTime(const TimerPerformance arg_performance)const
{		
	const float realRatio = 1.0f / fps;
	return GetTime(arg_performance) * realRatio;
}

float Timer::GetRate(const TimerPerformance arg_performance) const
{
	static float answer = 0;
	switch (arg_performance)
	{
	case TimerPerformance::Up:
		answer = Easing::Lerp(0, 1, nowCount / (float)limitCount);
		break;
	case TimerPerformance::Down:
		answer = Easing::Lerp(1, 0, nowCount / (float)limitCount);
		break;
	default:
		answer = Easing::Lerp(0, 1, nowCount / (float)limitCount);
		break;
	}
	return answer;
}

bool Timer::IsTime()const
{
	return nowCount >= limitCount;
}

void Timer::SetFPS(const float arg_fps)
{
	fps = arg_fps;
}
