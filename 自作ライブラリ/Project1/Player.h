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

	//接地判定の結果を入れる
	void SetOnGround(const bool arg_onGround);

	//コースアウトの結果を入れる
	void SetIsCourseOut(const bool arg_isCourseOut);

	//デバイスから受け取った帆の角度を代入(0が右、反時計回り)
	void SetAngle(const float arg_angle);
	//デバイスから受け取った風の強さを代入
	void SetPower(const float arg_power);

private:
	//帆入力による移動
	void MovePos_sail();
	//キーボード入力による移動(デバッグ用)
	void MovePos_key();
	//移動入力の切り替え
	bool isInputMode_sail = false;
	bool isInputMode_key = true;

	float DegreeToRadian(const float degree);

	FBXModel* myModel = nullptr;

	Vector3 pos_first = {};
	Vector3 pos_prev = {};
	const float speed_move = 1.0f;

	//地面との判定
	bool onGround = false;
	Vector3 gravity = {};
	const float gravity_acc = 0.02f;

	//コースアウト判定
	bool isCourseOut = false;

	//外部デバイスからの入力
	float angle = 0.0f;//帆の角度
	float power = 0.0f;//風の強さ

};
