#pragma once
#include "Object.h"
#include "FBXModel.h"

class Player : public Object
{
public:
	Player(const Vector3& arg_pos);
	~Player();
	void Initialize()override;
	void Update()override;
	void Draw() override;
	void DrawReady() override;

	//前方のベクトル
	Vector3 GetForwordVec();

	//接地判定の結果を入れる
	void SetOnGround(const bool arg_onGround);

	//コースアウトの結果を入れる
	void SetIsCourseOut(const bool arg_isCourseOut);

	//入力を受け付けるか
	void SetIsCanInput(const bool arg_isCanInput);
	bool GetIsCanInput();

	//デバイスから受け取った帆の角度を代入(0が右、反時計回り)
	void SetAngle(const float arg_angle);
	//デバイスから受け取った風の強さを代入
	void SetPower(const float arg_power);

private:
	//帆入力による移動
	void MovePos_sail();
	//キーボード入力による移動(デバッグ用)
	void MovePos_key();
	//入力がないときなどに減速をする
	void MovePos_brake();

	FBXModel* myModel = nullptr;

	Vector3 pos_first = {};
	Vector3 pos_prev = {};
	const float speed_move = 1.0f;
	Vector3 forwordVec = {};

	//地面との判定
	bool onGround = false;
	Vector3 gravity = {};
	const float gravity_acc = 0.02f;

	//コースアウト判定
	bool isCourseOut = false;

	//入力受付
	bool isCanInput = false;

	//外部デバイスからの入力
	float angle = 0.0f;//帆の角度
	float power = 0.0f;//風の強さ

};
