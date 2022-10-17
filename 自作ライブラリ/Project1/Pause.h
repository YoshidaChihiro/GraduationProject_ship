#pragma once
#include "Sprite.h"
#include "Vector.h"

class Pause
{
public:
	Pause();
	~Pause();

	void Initialize();
	void Update();
	void Draw();

	bool GetActivePause() { return activeFlag; }

	bool GetToGame() { return flag_toGame; }
	bool GetRestart() { return flag_restart; }
	bool GetToTitle() { return flag_toTitle; }

private:
	//選択肢を選ぶ処理
	void Select();
	void SelectSub_Sound();
	//決定ボタンを押した後の処理
	void Decision();

	//描画や更新処理を行うか
	bool activeFlag = false;
	//
	static bool fadeFlag;


	enum SelectState
	{
		ToGame = 0,
		Restart = 1,
		ToTitle = 2,
		Sound = 3,
	};
	SelectState selectState;
	//選択肢の個数
	const int selectMax = 4;

	enum SelectState_Sound
	{
		BGM = 0,
		SE = 1,
	};
	SelectState_Sound selectState_sound;
	//選択肢の個数
	const int selectMax_sound = 2;

	//X座標のずらす基準
	static const int positionStepMax = 4;
	static float positions_X[positionStepMax];

	//ゲーム画面を暗く
	Sprite* sp_back = nullptr;
	Vector2 pos_back = {};

	//選択を見やすく
	Sprite* sp_base = nullptr;
	Vector2 pos_base = {};
	float alpha_base = 1.0f;
	bool isUP_alphaChange = false;

	//選択肢1つに必要な変数
	struct SelectSprite
	{
		SelectSprite();
		~SelectSprite();
		void Initialize(const std::string& texName, const float posY);
		void Update();
		void Draw();

		//動き出す前準備
		void PreMoveSetting();

		Sprite* sprite = nullptr;
		std::string texName = "";
		Vector2 pos = {};
		Vector2 prevPos = {};
		int step = 0;
		int easeTime = 0;
	};


	//ゲームにもどる
	SelectSprite* toGame = nullptr;
	bool flag_toGame = false;

	//やり直す
	SelectSprite* restart = nullptr;
	bool flag_restart = false;

	//タイトルにもどる
	SelectSprite* toTitle = nullptr;
	bool flag_toTitle = false;

	//音量設定
	SelectSprite* sound = nullptr;
	bool flag_sound = false;//BGMとSEの設定を展開する
	//BGM
	SelectSprite* bgm = nullptr;
	Sprite* bar_bgm;
	Vector2 barPositionLeft_bgm;
	const Vector2 bar_scale = { 256.0f,5.0f };
	Sprite* circle_bgm;
	Vector2 circlePosition_bgm;
	//SE
	SelectSprite* se = nullptr;
	Sprite* bar_se;
	Vector2 barPositionLeft_se;
	Sprite* circle_se;
	Vector2 circlePosition_se;

	//操作説明
	Sprite* config_sp = nullptr;
};
