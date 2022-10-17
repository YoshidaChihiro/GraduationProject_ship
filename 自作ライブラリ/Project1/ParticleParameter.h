#pragma once
#include <intsafe.h>
#include <string>

#include"Vector.h"

struct ParticleParameter
{
	// 加速度
	Vector3 accel = {0,0,0};
	float s_alpha = 1;
	// スケール
	Vector2 scaleAce = { 0,0 };
	Vector2 s_scale = { 0,0 };
	Vector2 e_scale = { 0,0 };
	float e_alpha = 1;
	// 終了フレーム
	int num_frame = 60;

	Vector3 s_color = {0,0,0};
	float pad;
	Vector3 s_rotation = {0,0,0};
	float pad2;
	// 最終値
	Vector3 e_color = {1,1,1};
	float pad3;
	Vector3 e_rotation = {0,0,0};
	float pad4;
};

struct OutputData
{
	Vector3 position;
	//死亡フラグ
	UINT isDead;

	Vector3 rotation;
	//ビルボードするかどうか
	UINT billboardActive;

	DirectX::XMFLOAT4 color;
	Vector2 scale;
	Vector2 scaleVel;

	Vector3 velocity;
	UINT frame;
};
