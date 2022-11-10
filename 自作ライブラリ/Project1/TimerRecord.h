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
	void SettingPosition();

	bool isAction = false;

	Timer* timer = nullptr;


	NumberSprite* sprite_countDown = nullptr;
	XMFLOAT2 position_countDown = {};
	float seconds_countDown = 0.0f;


	NumberSprite* sprite_seconds = nullptr;
	XMFLOAT2 position_seconds = {};
	float seconds = 0.0f;

	NumberSprite* sprite_minutes = nullptr;
	XMFLOAT2 position_minutes = {};
	float minutes = 0.0f;

	NumberSprite* sprite_decimal = nullptr;
	XMFLOAT2 position_decimal = {};
	float decimal = 0.0f;

	static const int arrayCount_comma = 2;
	Sprite* sprite_comma[arrayCount_comma];
	XMFLOAT2 position_comma[arrayCount_comma] = {};

	Sprite* sprite_base = nullptr;
	XMFLOAT2 position_base = {};

	const int limit_default = 999 * 60;
	const int limit_countDown = 3 * 60;
	bool isCountDown = false;
	int time_prev = 0;

	bool isResult = false;
	int count_movePos = 0;
};

