#include "window.h"
#include <imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


Window::Window()
{
	window_width = 1920;
	window_height = 1080;
}

Window::Window(const int window_width, const int window_height):window_width(window_width),window_height(window_height)
{
}


Window::~Window()
{
}

LRESULT Window::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return 1;
	}

	//メッセージで分岐
	switch (msg)
	{
	case WM_DESTROY://ウィンドウが破棄された
		PostQuitMessage(0);//OSに対して、アプリの終了を伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//準備の処理を行う
}

void Window::CreateWidow(WINDOW_TYPE type)
{
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc; //ウィンドウプロシージャを設定
	w.lpszClassName = L"DirectXGame";//ウィンドウクラス名
	w.hInstance = GetModuleHandle(nullptr); //ウィンドウハンドル
	w.hCursor = LoadCursor(NULL, IDC_ARROW); //カーソル指定
			//ウィンドウクラスをOSに登録
	RegisterClassEx(&w);

	//ウィンドウサイズ{X座標 Y座標 横幅 縦幅}
	wrc = { 0,0,window_width,window_height };

	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//自動でサイズを補正

	switch (type)
	{
	case Window::WINDOW:
		hwnd = CreateWindow(w.lpszClassName,//クラス名
			L"GraduationProject_ship",			//タイトルバーの文字//
			WS_OVERLAPPEDWINDOW,	//標準的なウィンドウスタイル
			CW_USEDEFAULT,			//表示X座標（OSに任せる）
			CW_USEDEFAULT,			//表示Y座標（OSに任せる)
			wrc.right - wrc.left,	//ウィンドウ横幅
			wrc.bottom - wrc.top,	//ウィンドウ立幅
			nullptr,				//親ウィンドウハンドル
			nullptr,				//メニューハンドル
			w.hInstance,			//呼び出しアプリケーションハンドル
			nullptr);				//オプション
		break;
	case Window::FULLSCREEN:
		hwnd = CreateWindow(w.lpszClassName,//クラス名
			L"DirectXGame",			//タイトルバーの文字//
			WS_VISIBLE | WS_POPUP, //標準的なウィンドウスタイル
			0,	
			0,	
			window_width, //ウィンドウ横幅
			window_height, //ウィンドウ縦幅
			nullptr,				//親ウィンドウハンドル
			nullptr,				//メニューハンドル
			w.hInstance,			//呼び出しアプリケーションハンドル
			nullptr);				//オプション

		break;
	default:
		break;
	}

	//ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);
}

WNDCLASSEX Window::getW()
{
	return w;
}

HWND Window::GetHwnd()
{
	return hwnd;
}

int Window::GetWindowWidth()
{
	return window_width;
}

int Window::GetWindowHeight()
{
	return window_height;
}

void Window::End()
{
	// ウィンドウクラスを登録解除
	UnregisterClass(w.lpszClassName,w.hInstance);
}
