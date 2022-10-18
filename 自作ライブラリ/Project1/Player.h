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

	//デバイスから受け取った帆の角度を代入(0が右、反時計回り)
	void SetAngle(const float arg_angle);
	//デバイスから受け取った風の強さを代入
	void SetPower(const float arg_power);
	//
	bool GetIsSway();

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
	const float speed_move = 1.0f;

	//外部デバイスからの入力
	float angle = 0.0f;//帆の角度
	float power = 0.0f;//風の強さ
	//外部デバイスへの出力
	bool isSway = false;//ぶつかった時に振動させる

};
