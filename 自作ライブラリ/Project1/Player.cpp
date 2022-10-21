#include "Player.h"
#include "FBXManager.h"
#include"Input.h"
#include"DrawMode.h"
#include"imgui.h"
#include"CollisionManager.h"

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
	pos_prev = position;
	rotation = {};
	velocity = {};

	onGround = false;
	gravity = {};

	isCourseOut = false;

	angle = 90.0f;
	power = 0.01f;
	isSway = false;
}

void Player::Update()
{
	pos_prev = position;

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

	//����
	if (!onGround)
	{
		gravity.y -= gravity_acc;
		position += gravity;
	}
	else
	{
		gravity = {};
	}

	Object::Update();
}

void Player::Draw()
{
	Object::CustomDraw(true, true);
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
		ImGui::DragFloat("power_wind", &power, 0.01f, 0.0f, 1.0f);
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

void Player::SetOnGround(const bool arg_onGround)
{
	//�߂�
	if (!onGround && arg_onGround)
	{
		position = pos_prev;
	}

	onGround = arg_onGround;
}

void Player::SetIsCourseOut(const bool arg_isCourseOut)
{
	//�߂�
	if (!isCourseOut && arg_isCourseOut)
	{
		position = pos_prev;
	}
	isCourseOut = arg_isCourseOut;
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
	//��]������
	const float power_rotation = angle - 90.0f;
	const float speed_rotation = 0.01f;
	rotation.z += power_rotation * speed_rotation;

	//�O�����ɐi��(���[�J�����W��Z+����)
	velocity.x = Object::GetMatWorld().r[1].m128_f32[0];
	velocity.z = Object::GetMatWorld().r[1].m128_f32[1];

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
