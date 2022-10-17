#include "Player.h"
#include "FBXManager.h"
#include"Input.h"
#include"DrawMode.h"
#include"imgui.h"

Player::Player(const Vector3& arg_pos)
{
	//アニメーション用にモデルのポインタを格納
	myModel = FBXManager::GetModel("ship");
	//モデルの生成
	Create(myModel);

	name = typeid(*this).name();

	pos_first = arg_pos;
	Initialize();
}

Player::~Player()
{
}

void Player::Initialize()
{
	pos = pos_first;
	rotation = {};
	velocity = {};

	angle = 90.0f;
	power = 0.1f;
	isSway = false;
}

void Player::Update()
{
	//移動
	MovePos_mast();
	//MovePos_key();

	rotation = { 0.0f, 0.0f, angle - 90.0f };
	Object::SetRotation(rotation);
	Object::Update();
}

void Player::Draw()
{
	Object::CustomDraw(true);
}

void Player::DrawReady()
{
#ifdef _DEBUG
	if (!Object3D::GetDrawShadow() && DrawMode::GetDrawImGui())
	{
		ImGui::Begin("PlayerStatus");
		ImGui::InputFloat("angle_mast", &angle);
		ImGui::InputFloat("power_wind", &power);
		ImGui::Text("angle_mast : %f\n", angle);
		ImGui::Text("power_wind : %f\n", power);
		ImGui::Checkbox("isSway\n", &isSway);
		ImGui::End();
	}

#endif

	if (Object3D::GetDrawShadow())
	{
		pipelineName = "FBXShadowMap";
	}
	else
	{
		pipelineName = "FBX";
	}
}

void Player::SetAngle(const float arg_angle)
{
	angle = arg_angle;
}

void Player::SetPower(const float arg_power)
{
	power = arg_power;
}

bool Player::GetIsSway()
{
	return isSway;
}

void Player::MovePos_mast()
{
	const float angle_radian = DegreeToRadian(angle);
	//角度をxzベクトルに変換
	velocity = {};
	velocity.x = cos(angle_radian);
	velocity.z = sin(angle_radian);

	pos += velocity * speed_move * power;
	Object::SetPosition(pos);
}

void Player::MovePos_key()
{
	velocity = {};
	if (Input::DownKey(DIK_UP))
	{
		velocity.z = 1.0f;
	}
	if (Input::DownKey(DIK_DOWN))
	{
		velocity.z = -1.0f;
	}
	if (Input::DownKey(DIK_RIGHT))
	{
		velocity.x = 1.0f;
	}
	if (Input::DownKey(DIK_LEFT))
	{
		velocity.x = -1.0f;
	}

	pos += velocity * speed_move;
	Object::SetPosition(pos);
}

float Player::DegreeToRadian(const float degree)
{
	return degree * (3.141592654f / 180.0f);
}
