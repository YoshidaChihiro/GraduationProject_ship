#pragma once
#include "Timer.h"
#include "NumberSprite.h"

class TimerRecord
{
public:
	TimerRecord();
	~TimerRecord();

	void Initialize();
	void Update();
	void Draw();

	void Start();
	void Goal();

	bool GetIsAction();

private:
	bool isAction = false;

	Timer* timer = nullptr;

	NumberSprite* sprite_seconds;
	float seconds = 0.0f;

	const int limit_default = 999 * 60;
	const int limit_countDown = 3 * 60;
	bool isCountDown = false;

};

