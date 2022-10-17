#include "ScreenCamera.h"
#include"Input.h"
#include"Object.h"
ScreenCamera::ScreenCamera() :Camera({480,270})
{
	phi = -3.14159265f / 2;
	theta = 120;
}


void ScreenCamera::Update()
{
	bool dirty = true;

	auto mouseMove = Input::GetMouseMove();

	//// ゲームパッドの右スティックでのカメラ操作
	//if (Input::CheckPadRStickLeft()|| Input::CheckPadRStickUp() || Input::CheckPadRStickRight() || Input::CheckPadRStickDown())
	//{

	//	auto vec = Input::GetRStickDirection();

	//	phi += XM_PI / 180.0f * -vec.x * ((float)SettingParam::GetPadSensitive() / 3.0f) * SettingParam::GetReverseX();
	//	theta += XM_PI / 180.0f * -vec.y * ((float)SettingParam::GetPadSensitive() / 3.0f) * SettingParam::GetReverseY();
	//	if (theta > 40 * XM_PI / 180.0f)
	//		theta = 40 * XM_PI / 180.0f;
	//	else if (theta < -40 * XM_PI / 180.0f)
	//		theta = -40 * XM_PI / 180.0f;

	//	if (phi > 360 * XM_PI / 180.0f)
	//		phi -= 360 * XM_PI / 180.0f;
	//	else if (phi < 0)
	//		phi += 360 * XM_PI / 180.0f;

	//	if (theta < 0)
	//	{
	//		distance = 10 * (1 + theta * 1.1f);

	//	}
	//	dirty = true;
	//}


	// マウスの左ボタンが押されていたらカメラを回転させる
	if (Input::DownMouseKey(Input::LEFT))
	{

		phi += XM_PI / 180.0f * mouseMove.lX / 7;
		theta += XM_PI / 180.0f * mouseMove.lY / 7;
		if (theta > 40 * XM_PI / 180.0f)
			theta = 40 * XM_PI / 180.0f;
		else if (theta < -40 * XM_PI / 180.0f)
			theta = -40 * XM_PI / 180.0f;

		if (phi > 360 * XM_PI / 180.0f)
			phi -= 360 * XM_PI / 180.0f;
		else if (phi < 0)
			phi += 360 * XM_PI / 180.0f;

		//printf("phi : %f , theta : %f\n", phi, theta);
		dirty = true;
	}

	if(targetObj)
		target = targetObj->GetPosition() + Vector3{0, 1, 0};

	// ホイール入力で距離を変更
	if (mouseMove.lZ != 0) {
		distance -= mouseMove.lZ / 100.0f;
		distance = max(distance, 1.0f);
		dirty = true;
	}

	if (dirty || viewDirty) {
		float nowTheta = theta;
		if (theta < 0)
			nowTheta = 0;
		eye = Vector3(cos(phi) * cos(nowTheta), sin(nowTheta), sin(phi) * cos(nowTheta)) * distance + target;
		viewDirty = true;

	}

	Camera::Update();

}
