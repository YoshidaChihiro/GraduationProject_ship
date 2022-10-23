#include "InGameCamera.h"

#include <cstdio>
#include <random>
#include <algorithm>

#include "GameSettingParam.h"
#include "Vector.h"
#include "window.h"

#include "Player.h"


InGameCamera::InGameCamera()
{
	phi = -3.14159265f / 2;
	theta = 0;
}

void InGameCamera::Initialize()
{
	//auto fields = ActorManager::GetInstance()->GetFields();
	//if (fields.empty())
	//{
	//	SetTarget(Vector3(0, -5, 0));
	//}
	//else
	//{
	//	SetTarget(fields[0]->GetPosition());
	//}
	SetPhi(DirectX::XMConvertToRadians(-90));
	SetTheta(120);
	SetDistance(35);
	Update();
}

void InGameCamera::Update()
{
	dirty = true;

	if (dirty || viewDirty) 
	{
		float nowTheta = theta;
		if (theta < 0)
			nowTheta = 0;
		eye = Vector3(cos(phi) * cos(nowTheta), sin(nowTheta), sin(phi) * cos(nowTheta)) * distance + target;
		viewDirty = true;

	}

	if (shakeFlag)
	{
		Shake();
	}
	

	Camera::Update();
}

void InGameCamera::RotateYaxis(Vector2 arg_inputVec)
{
	auto vec = arg_inputVec;
	phi += XM_PI / 180.0f * -vec.x * ((float)SettingParam::GetPadSensitive() / 3.0f) * SettingParam::GetReverseX();
	/*theta += XM_PI / 180.0f * -vec.y * ((float)SettingParam::GetPadSensitive() / 3.0f) * SettingParam::GetReverseY();
	if (theta > 40 * XM_PI / 180.0f)
		theta = 40 * XM_PI / 180.0f;
	else if (theta < -40 * XM_PI / 180.0f)
		theta = -40 * XM_PI / 180.0f;*/

	if (phi > 360 * XM_PI / 180.0f)
		phi -= 360 * XM_PI / 180.0f;
	else if (phi < 0)
		phi += 360 * XM_PI / 180.0f;

	/*if (theta < 0)
	{
		distance = 10 * (1 + theta * 1.1f);

	}*/
	dirty = true;
}

void InGameCamera::SetShake(const int arg_shakeTime, const float arg_shakePower)
{
	shakeTimer = 0;
	shakeTime = arg_shakeTime;
	shakePower = arg_shakePower;

	shakeStartEyePos = GetEye();
	shakeStartTargetPos = GetTarget();

	shakeFlag = true;
}

void InGameCamera::Shake()
{
	Vector3 shakeDirection{};
	shakeDirection.x = GetIntRand(0, 10) - 5;
	shakeDirection.y = GetIntRand(0, 10) - 5;
	shakeDirection.z = 0.0f;
	shakeDirection.Normalize();

	eye = shakeStartEyePos + shakeDirection * shakePower;
	target = shakeStartTargetPos + shakeDirection * shakePower;

	shakeTimer++;
	if (shakeTimer == shakeTime)
	{
		shakeFlag = false;
		eye = shakeStartEyePos;
		target = shakeStartTargetPos;
	}
}

int InGameCamera::GetIntRand(int minValue, int maxValue)
{
	std::random_device rnd;
	std::mt19937_64 mt64(rnd());
	std::uniform_int_distribution<int> genRandFloat(minValue, maxValue);
	return genRandFloat(mt64);
}
