#pragma once
#include <Windows.h>

class Window
{
public:
	enum WINDOW_TYPE
	{
		WINDOW,
		FULLSCREEN,
	};
	Window();
	Window(const int window_width, const int window_height);
	~Window();
	void CreateWidow(WINDOW_TYPE type);
	WNDCLASSEX getW();
	//ウィンドウサイズ
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	HWND GetHwnd();
	int GetWindowWidth();
	int GetWindowHeight();
	void End();
private:
	int window_width;//横幅
	int window_height;//横幅
	RECT wrc;
	WNDCLASSEX w{};  //ウィンドウクラスの設定
	//ウィンドウオブジェクトの生成
	HWND hwnd;



};

