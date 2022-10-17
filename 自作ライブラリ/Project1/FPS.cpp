#include "FPS.h"

#include <chrono>
#include <time.h>
#include<synchapi.h>
#include<stdio.h>

#include "Timer.h"

float FPS::fps = 60;
//int FPS::count = 0;
//DWORD FPS::startProsessTime = 0;
//DWORD FPS::nowTime = 0;
// float FPS::deltaTime = 0;
// float FPS::frameRate = 0;
// std::chrono::steady_clock::time_point FPS::lastUpdate = {};
int FPS::fpsCount = 0;

 Window* FPS::win = nullptr;

  LARGE_INTEGER FPS::mTimeEnd = {};
  LARGE_INTEGER FPS::mTimeFreq = {};
  LARGE_INTEGER FPS::mTimeStart = {};
  std::chrono::steady_clock::time_point FPS::lastUpdate = {};

void FPS::Initialize(const float & fps)
{
	FPS::fps = fps;
	//startProsessTime = timeGetTime();
	//nowTime = timeGetTime();

	Timer::SetFPS(fps);
}

void FPS::StartMeasure()
{
	//周波数取得
	QueryPerformanceFrequency(&mTimeFreq);
	//計測開始時間の初期化
	QueryPerformanceCounter(&mTimeStart);


}

void FPS::FPSFixed()
{
	// 今の時間を取得
	QueryPerformanceCounter(&mTimeEnd);
	// (今の時間 - 前フレームの時間) / 周波数 = 経過時間(秒単位)
	auto mFrameTime = static_cast<double>(mTimeEnd.QuadPart - mTimeStart.QuadPart) / static_cast<double>(mTimeFreq.QuadPart);

	

#if _DEBUG

	fpsCount++;
	static int count = 0;
	// 経過時間計測
	auto now = std::chrono::steady_clock::now();
	float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - lastUpdate).count() / 1000000.0f;
	float frameRate = 1.0f / deltaTime;
	lastUpdate = now;

	if (fpsCount == 60)
	{

		//char str[50];
		char str[50];
		sprintf_s(str, 50, "fps=%f", frameRate);
		SetWindowTextA(win->GetHwnd(), str);

		//printf("FPS:",);
		fpsCount = 0;
	}
#endif // _DEBUG
	//処理時間に余裕がある場合はその分待つ
	auto FRAME_TIME = 1.0 / fps;

	if (mFrameTime < FRAME_TIME)
	{
		//Sleepの時間を計算
		DWORD sleepTime = static_cast<DWORD>((FRAME_TIME - mFrameTime) * 1000);
		timeBeginPeriod(1);
		//寝る
		Sleep(sleepTime);
		timeEndPeriod(1);
	}

}
