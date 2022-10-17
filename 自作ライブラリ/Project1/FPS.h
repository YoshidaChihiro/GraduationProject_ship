#pragma once
#include <chrono>
#include <Windows.h>

#include "window.h"
#pragma comment(lib, "winmm.lib")
class FPS
{
public://静的メンバ関数
	static void Initialize(const float& fps = 60);
	static void StartMeasure();
	static void FPSFixed();
	static void SetWindow(Window* win) { FPS::win = win; }
private://静的メンバ変数
	static Window* win;
	static float fps;
	//static int count;
	//static DWORD startProsessTime;
	//static DWORD nowTime;
	static int fpsCount;

	//static float deltaTime ;
	//static float frameRate ;
	//static std::chrono::steady_clock::time_point lastUpdate;
	static LARGE_INTEGER mTimeEnd;
	static LARGE_INTEGER mTimeFreq; 
	static LARGE_INTEGER mTimeStart;
	static std::chrono::steady_clock::time_point lastUpdate;
};

