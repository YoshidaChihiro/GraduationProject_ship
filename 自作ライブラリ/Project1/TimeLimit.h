#pragma once
#include "Timer.h"
#include "NumberSprite.h"
#include "Vector.h"
#include<unordered_map>

class TimeLimit
{
public:
	TimeLimit(const unsigned int arg_limit);//引数の単位はフレーム
	~TimeLimit();

	void Initialize();
	void Update();
	void Draw();

	bool GetLimit() { return timer->IsTime(); }
	int GetNowTime() { return timer->GetTime(); }

private:
	void Alarm(const int arg_nowTime);

	int limit;
	float minute;
	float seconds;

	Timer* timer;

	//分表示
	NumberSprite* minute_sprite;
	//コロン
	Sprite* colon_sprite;
	//秒表示
	NumberSprite* seconds_sprite;

	Vector4 color;

	//残り時間警告
	Sprite* alarm_sprite;
	Vector2 alarm_position{};
	const Vector2 alarm_position_first = { 1920 + 320, 1080 / 2 };
	struct AlarmTime
	{
		int time;
		std::string texName;
	};
	int timeLeft = 0;
	//警告を出す時間
	static const int alarmTime_count = 3;
	const AlarmTime alarmTime[alarmTime_count] =
	{
		{30, "GamePlay_UI_TimeLimit_30"},
		{60, "GamePlay_UI_TimeLimit_60"},
		{90, "GamePlay_UI_TimeLimit_90"},
		//{120, "GamePlay_UI_TimeLimit_120"},
		//{150, "GamePlay_UI_TimeLimit_150"}
	};
	bool isMoveStart = false;
	float easeMoveCount = 0.0f;

	//ラスト10秒版
	bool isRedTime = false;
	const int redTime = 10;
	NumberSprite* redNumber_sprite;

};

