#pragma once
#include "Object.h"
#include "OBJModel.h"

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

	//壁や障害物にぶつかった時
	void HitObstacle();
	bool GetIsHitObstacle();
	//各初期化
	void InitBackStep();
	void InitSlipY();
	void InitSlipZ();

	//入力を受け付けるか
	void SetIsCanInput(const bool arg_isCanInput);

	//デバイスから受け取った帆の角度を代入
	void SetAngle(const float arg_angle_device);
	//デバイスから受け取った風の強さを代入
	void SetPower(const float arg_power_device);

	float GetPower();

private:
	//帆入力による移動
	void MovePos_sail();
	//キーボード入力による移動(デバッグ用)
	void MovePos_key();

	//帆入力の値に寄せていく
	void MovePos_linear();

	//壁や障害物との衝突時
	void HitActions();
	bool BackStep(const int arg_limit);
	bool SlipY(const int arg_limit);
	bool SlipZ(const int arg_limit, const int arg_countTotal);


	OBJModel* myModel = nullptr;

	Vector3 pos_first = {};
	Vector3 pos_prev = {};
	const float speed_move = 1.0f;
	Vector3 forwordVec = {};

	//地面との判定
	bool onGround = false;
	Vector3 gravity = {};
	const float gravity_acc = 0.02f;

	//障害物衝突判定
	bool isHitObstacle = false;
	//跳ね返り
	int count_backStep = 0;
	Vector3 pos_backStep_start = {};
	Vector3 pos_backStep_end = {};
	//回転Y
	int count_slipY = 0;
	float rotation_slipY_start = 0.0f;
	float rotation_slipY_end = 0.0f;
	//回転Z
	int count_slipZ = 0;
	float rotation_slipZ_start = 0.0f;
	float rotation_slipZ_end = 0.0f;
	int count_slipZ_end = 0;

	//入力受付
	bool isCanInput = false;

	//外部デバイスからの入力
	float angle_device = 0.0f;//帆の角度
	float power_device = 0.0f;//風の強さ

	float angle = 0.0f;//帆の角度
	float power = 0.0f;//風の強さ

};
