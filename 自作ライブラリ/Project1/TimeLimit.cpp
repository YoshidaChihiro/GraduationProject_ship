#include "TimeLimit.h"
#include "Vector.h"
#include "Easing.h"

TimeLimit::TimeLimit(const unsigned int arg_limit)
{
	limit = arg_limit;
	minute = limit / 60;
	seconds = limit % 60;
	timer = new Timer(arg_limit);

	minute_sprite = new NumberSprite(minute);
	colon_sprite = new Sprite();
	seconds_sprite = new NumberSprite(seconds);

	alarm_sprite = new Sprite();

	redNumber_sprite = new NumberSprite(seconds);
}

TimeLimit::~TimeLimit()
{
	delete timer;
	delete minute_sprite;
	delete colon_sprite;
	delete seconds_sprite;
	delete alarm_sprite;
	delete redNumber_sprite;
}

void TimeLimit::Initialize()
{
	timer->SetLimit(limit, true);
	color = { 1,1,1,1.0f };

	alarm_position = alarm_position_first;
	timeLeft = 0;
	isMoveStart = false;
	easeMoveCount = 0.0f;
	isRedTime = false;
}

void TimeLimit::Update()
{
	//残り時間（秒）
	const int nowTime = timer->GetRealTime(TimerPerformance::Down);
	//表示時間（分）
	minute = nowTime / 60;
	//表示時間（秒）
	seconds = nowTime % 60;

	timer->Update();

	//残り時間の警告
	Alarm((timer->GetLimit() - timer->GetTime()) / 60);

	//終了間際に色を変える
	isRedTime = nowTime <= redTime;
	if (isRedTime)
	{
		color.x = 1.0f;
		color.y = 0.1f;
		color.z = 0.1f;
	}
}

void TimeLimit::Draw()
{
	if (minute < 1 && seconds < 1)
	{
		return;
	}

	//画像サイズ（数字1つ分）
	const Vector2 numberTexSize = { 47.0f, 86.0f };
	//画面サイズ
	const Vector2 windowSize = { 1920.0f, 1080.0f };

	//座標
	Vector2 position = { windowSize.x / 2, windowSize.y / 4};

	if (isRedTime)
	{
		std::string nuwNum = std::to_string((int)seconds);
		redNumber_sprite->Draw(nuwNum.size(), "GamePlay_UI_Number", position, { 2,2 }, color);
		return;
	}


	position.x -= numberTexSize.x;//分の中心にずらす
	position.y -= numberTexSize.y / 2;
	minute_sprite->Draw(1, "GamePlay_UI_Number", position, { 1,1 }, color/*, { 0.5f,0.5f },"NoAlphaToCoverageSprite"*/);

	position.x += (numberTexSize.x / 4) * 3;//コロンの中心にずらす
	colon_sprite->DrawSprite("GamePlay_UI_Colon", position, 0, { 1,1 }, color/*, { 0.5f,0.5f }, "NoAlphaToCoverageSprite"*/);

	position.x += (numberTexSize.x / 4) * 5;//秒の中心にずらす
	seconds_sprite->Draw(2, "GamePlay_UI_Number", position, { 1,1 }, color/*, { 0.5f,0.5f }, "NoAlphaToCoverageSprite"*/);

	alarm_sprite->DrawSprite(alarmTime[timeLeft].texName, alarm_position);
}

void TimeLimit::Alarm(const int arg_nowTime)
{
	//区切りの時間になったか
	for (int i = 0; i < alarmTime_count; i++)
	{
		if (alarmTime[i].time == arg_nowTime)
		{
			timeLeft = i;
			isMoveStart = true;
		}
	}

	//移動
	if (isMoveStart)
	{
		const float endPositionX_first = 1920.0f / 2.0f;
		const float endPositionX_end = -320.0f;

		const float easeLimit = 3.0f * 60.0f;
		//前半
		if (easeMoveCount <= easeLimit / 2.0f)
		{
			alarm_position.x = Easing::EaseOutQuad(alarm_position_first.x, endPositionX_first, easeLimit / 2.0f, easeMoveCount);
		}
		//後半
		else
		{
			alarm_position.x = Easing::EaseInQuad(endPositionX_first, endPositionX_end, easeLimit / 2.0f, easeMoveCount - (easeLimit / 2.0f));
		}
		easeMoveCount++;
	}
	//移動終了後
	if (alarm_position.x < 0.0f)
	{
		isMoveStart = false;
		easeMoveCount = 0.0f;
		alarm_position = alarm_position_first;
	}
}
