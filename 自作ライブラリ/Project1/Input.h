#pragma once
#define DIRECTINPUT_VERSION 0x0800 //DirectInputのバージョン
#include <array>
#include <dinput.h>
#include <Windows.h>
#include <wrl.h>
#include<DirectXMath.h>
#include <Xinput.h>

#pragma comment(lib, "xinput.lib")
//https://docs.microsoft.com/en-us/windows/win32/api/xinput/ns-xinput-xinput_gamepad
struct MouseMove
{
	LONG lX;
	LONG lY;
	LONG lZ;
};
class Input
{
	struct float2
	{
		float x;
		float y;
	};
private:
template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	enum MOUSEBOTTAN
	{
		LEFT,
		RIGHT,
		MIDDLE
	};
	
	static void Update();
	static bool TriggerKey(BYTE keyCode);//キーボードのトリガー判定
	static bool DownKey(BYTE keyCode);//キーボードの押している間反応
	static bool UpKey(BYTE keyCode);//キーボードの離したとき反応
	static bool TriggerMouseKey(MOUSEBOTTAN bottan);//マウスのトリガー判定
	static bool DownMouseKey(MOUSEBOTTAN bottan);//マウスの押している間反応
	static bool UpMouseKey(MOUSEBOTTAN bottan);//マウスの離したとき反応
	static MouseMove GetMouseMove();
	static void Initialize(HWND hwnd);
	static DirectX::XMFLOAT2 GetMousePos() { 
		return mousePos;
	};

	//ゲームパッド
	static bool CheckPadButton(int button);
	static bool TriggerPadButton(int button);
	static bool UpPadButton(int button);

	static bool CheckPadRightTrigger(); //右トリガー
	static bool CheckPadLeftTrigger();  //左トリガー

	static bool TriggerPadRightTrigger(); //右トリガーのトリガー判定
	static bool TriggerPadLeftTrigger(); //左トリガーのトリガー判定
	//右スティック
	static bool CheckPadRStickRight();
	static bool CheckPadRStickLeft();
	static bool CheckPadRStickUp();
	static bool CheckPadRStickDown();
	static bool CheckPadRStickAnythingDir();

	static bool TriggerPadRStickRight();
	static bool TriggerPadRStickLeft();
	static bool TriggerPadRStickUp();
	static bool TriggerPadRStickDown();

	//左スティック
	static bool CheckPadLStickRight();
	static bool CheckPadLStickLeft();
	static bool CheckPadLStickUp();
	static bool CheckPadLStickDown();
	static bool CheckPadLStickAnythingDir();

	static bool TriggerPadLStickRight();
	static bool TriggerPadLStickLeft();
	static bool TriggerPadLStickUp();
	static bool TriggerPadLStickDown();

	static float2 GetLStickDirection();
	static float2 GetRStickDirection();

	//ゲームパッド振動
	static void LeftVibration(unsigned int power, int setTime);
	static void RightVibration(unsigned int power, int setTime);
	static void Vibration(unsigned int power, int setTime);

	//何かのボタンが押されたら押されたボタンを返す
	static int CheckAnyPadButton();
	
	static void ChangeMouseMove()
	{
		if (mouseMove == true)
		{
			while (1)
			{
				if (ShowCursor(false) < 0)
					break;
			}
			mouseMove = false;
		}
		else
		{
			while (1)
			{
				if (ShowCursor(true) >= 0)
					break;
			}
			mouseMove = true;
		}
	}

	//移動に使いそうなキー・スティック入力があるか
	static bool DownWASD();
	static bool DownArrow();

private:
	static ComPtr<IDirectInputDevice8> devkeyboard;
	static ComPtr<IDirectInputDevice8> devPad;
	static ComPtr<IDirectInputDevice8> devMouse;
	static std::array<BYTE,256> keys;
	static std::array<BYTE, 256> prevKeys;
	static DIMOUSESTATE2 mouseState ;
	static DIMOUSESTATE2 mouseStatePre;
	static int num;//パッドの初期化に使用
	static int pad_LStickX, pad_LStickY;
	static int prev_LStickX, prev_LStickY;
	static DirectX::XMFLOAT2 mousePos;
	
	static XINPUT_STATE padState;
	static XINPUT_STATE prevPadState;
	static XINPUT_VIBRATION vibration;
	static int time;

	static int screenWidth;
	static int screenHeight;

	static bool mouseMove;
};

