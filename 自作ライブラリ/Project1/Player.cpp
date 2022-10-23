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
	power = 0.1f;
}

void Player::Update()
{
	//�����Ԃ��p�ɑO�t���[���̍��W���i�[
	pos_prev = position;

	//�ړ�
	MovePos_sail();
#ifdef _DEBUG
	MovePos_key();
#endif

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

	//�O���̃x�N�g�����i�[
	forwordVec = {
		Object::GetMatWorld().r[2].m128_f32[0],
		Object::GetMatWorld().r[2].m128_f32[1],
		Object::GetMatWorld().r[2].m128_f32[2]
	};
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
		ImGui::Text("rotation : %f, %f, %f\n", rotation.x, rotation.y, rotation.z);
		ImGui::End();

		ImGui::Begin("DeviceInformation");
		ImGui::DragFloat("angle_mast", &angle, 1.0f, 0.0f, 180.0f);
		ImGui::DragFloat("power_wind", &power, 0.01f, 0.0f, 1.0f);
		ImGui::Text("angle_mast : %f\n", angle);
		ImGui::Text("power_wind : %f\n", power);
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

Vector3 Player::GetForwordVec()
{
	return forwordVec;
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

void Player::MovePos_sail()
{
	//��]������
	const float power_rotation = angle - 90.0f;
	const float speed_rotation = 0.01f;
	rotation.y += power_rotation * speed_rotation;

	if (rotation.y < 0.0f)
	{
		rotation.y += 360.0f;
	}
	else if (rotation.y > 360.0f)
	{
		rotation.y -= 360.0f;
	}

	//�O�����ɐi��
	velocity = forwordVec;

	position += velocity * speed_move * power;
}

void Player::MovePos_key()
{
	//��]����
	const float angle_key = 1.0f;//��]
	if (Input::DownKey(DIK_RIGHT) && angle < 180.0f)
	{
		angle += angle_key;
	}
	if (Input::DownKey(DIK_LEFT) && angle > 0.0f)
	{
		angle -= angle_key;
	}

	//���x����
	const float power_key = 0.01f;//�����x
	if (Input::DownKey(DIK_UP))
	{
		power += power_key;
	}
	if (Input::DownKey(DIK_DOWN))
	{
		power -= power_key;
	}
}

float Player::DegreeToRadian(const float degree)
{
	return degree * (3.141592654f / 180.0f);
}
