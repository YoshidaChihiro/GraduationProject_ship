#pragma once
#include "FPS.h"

enum class TimerPerformance
{
	Up,
	Down,
};

class Timer
{
public:
	Timer();
	Timer(const unsigned int arg_limit);
	~Timer();

	void Initialize();
	void Update();
	
	void Reset();	
	int GetLimit()const;
	void SetLimit(const unsigned int arg_limit, const bool arg_isReset = false);
	//経過時間（フレーム）
	int GetTime(const TimerPerformance arg_performance = TimerPerformance::Up)const;
	//経過時間（秒）
	float GetRealTime(const TimerPerformance arg_performance = TimerPerformance::Up)const;
	//経過率
	float GetRate(const TimerPerformance arg_performance = TimerPerformance::Up)const;
	
	bool IsTime()const;
	
	static void SetFPS(const float arg_fps);

private:
	unsigned int nowCount;
	unsigned int limitCount;
	static float fps;
};