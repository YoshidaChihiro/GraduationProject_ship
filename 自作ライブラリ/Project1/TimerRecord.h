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

	const XMFLOAT2 position_center = { 1920 / 2, 1080 / 2 };
	const XMFLOAT2 position_inGame = { 1920 / 2, 64 };
	const float size_numberX = 47;

	NumberSprite* sprite_countDown = nullptr;
	const XMFLOAT2 position_countDown = position_center;
	float seconds_countDown = 0.0f;


	NumberSprite* sprite_seconds = nullptr;
	const XMFLOAT2 position_seconds = position_inGame;
	float seconds = 0.0f;

	NumberSprite* sprite_minutes = nullptr;
	const XMFLOAT2 position_minutes = {
		position_inGame.x - (size_numberX * 3),
		position_inGame.y
	};
	float minutes = 0.0f;

	NumberSprite* sprite_decimal = nullptr;
	const XMFLOAT2 position_decimal = {
	position_inGame.x + (size_numberX * 3),
	position_inGame.y
	};
	float decimal = 0.0f;

	static const int arrayCount_comma = 2;
	Sprite* sprite_comma[arrayCount_comma];
	const XMFLOAT2 position_comma[arrayCount_comma] = {
		{ position_inGame.x - (size_numberX * 1.5f), position_inGame.y },
		{ position_inGame.x + (size_numberX * 1.5f), position_inGame.y },
	};

	Sprite* sprite_base = nullptr;

	const int limit_default = 999 * 60;
	const int limit_countDown = 3 * 60;
	bool isCountDown = false;

};

