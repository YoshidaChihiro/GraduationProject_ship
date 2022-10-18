#include "Player.h"
#include "FBXManager.h"
#include"Input.h"
#include"DrawMode.h"
#include"imgui.h"

Player::Player(const Vector3& arg_pos)
{
	//�A�j���[�V�����p�Ƀ��f���̃|�C���^���i�[
	myModel = FBXManager::GetModel("ship");
	//���f���̐���
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
	position = pos_first;
	rotation = {};
	velocity = {};

	angle = 90.0f;
	power = 0.1f;
	isSway = false;
}

void Player::Update()
{
	//�ړ�
	isInputMode_sail = !isInputMode_key;
	if (isInputMode_sail)
	{
		MovePos_sail();
	}
	if (isInputMode_key)
	{
		MovePos_key();
	}

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
		ImGui::Begin("PlayerStates");
		ImGui::Text("position : %f, %f, %f\n", position.x, position.y, position.z);
		ImGui::Text("velocity : %f, %f, %f\n", velocity.x, velocity.y, velocity.z);
		ImGui::Checkbox("inputmode : key\n", &isInputMode_key);
		ImGui::End();

		ImGui::Begin("DeviceInformation");
		ImGui::DragFloat("angle_mast", &angle, 1.0f, 0.0f, 180.0f);
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

void Player::MovePos_sail()
{
	const float angle_radian = DegreeToRadian(angle);
	//�p�x��xz�x�N�g���ɕϊ�
	velocity.x = cos(angle_radian);
	velocity.z = sin(angle_radian);

	position += velocity * speed_move * power;
}

void Player::MovePos_key()
{
	const float power_key = 0.05f;//�����x
	const float antiPower_key = 0.03f;//�����x

	//�O��ړ�
	if (Input::DownKey(DIK_UP))
	{
		velocity.z += power_key;
	}
	if (Input::DownKey(DIK_DOWN))
	{
		velocity.z -= power_key;
	}
	//����
	if (!Input::DownKey(DIK_UP) && !Input::DownKey(DIK_DOWN) && velocity.z != 0.0f)
	{
		int pm = 1;//�v���X���}�C�i�X��
		if (velocity.z > 0.0f)
		{
			pm = -1;
		}
		velocity.z += antiPower_key * pm;
	}

	//���E�ړ�
	if (Input::DownKey(DIK_RIGHT))
	{
		velocity.x += power_key;
	}
	if (Input::DownKey(DIK_LEFT))
	{
		velocity.x -= power_key;
	}
	//����
	if (!Input::DownKey(DIK_RIGHT) && !Input::DownKey(DIK_LEFT) && velocity.x != 0.0f)
	{
		int pm = 1;//�v���X���}�C�i�X��
		if (velocity.x > 0.0f)
		{
			pm = -1;
		}
		velocity.x += antiPower_key * pm;
	}

	//�ő呬�x�ɐ���
	const float speed_max = 10.0f;//�ő呬�x
	if (velocity.z > speed_max)
	{
		velocity.z = speed_max;
	}
	else if (velocity.z < -speed_max)
	{
		velocity.z = -speed_max;
	}
	if (velocity.x > speed_max)
	{
		velocity.x = speed_max;
	}
	if (velocity.x < -speed_max)
	{
		velocity.x = -speed_max;
	}
	//�덷�͈̔͂Œ�~
	const float deadzone = antiPower_key;
	if (velocity.z < deadzone && velocity.z > -deadzone)
	{
		velocity.z = 0.0f;
	}
	if (velocity.x < deadzone && velocity.x > -deadzone)
	{
		velocity.x = 0.0f;
	}

	//���W�ύX
	position += velocity * speed_move;
}

float Player::DegreeToRadian(const float degree)
{
	return degree * (3.141592654f / 180.0f);
}
