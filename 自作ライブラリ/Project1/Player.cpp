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

	isCanInput = false;

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
	if (!isCanInput)
	{
		MovePos_brake();
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
		ImGui::DragFloat("angle_mast", &angle, 1.0f, 89.0f, 90.0f);
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

void Player::SetIsCanInput(const bool arg_isCanInput)
{
	isCanInput = arg_isCanInput;
}

bool Player::GetIsCanInput()
{
	return isCanInput;
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
	int abs_rotation = 0;//��]�̌���
	if (angle > 90.0f)
	{
		abs_rotation = 1;
	}
	else if (angle < 90.0f)
	{
		abs_rotation = -1;
	}
	const float speed_rotation = 0.1f;
	rotation.y += power * speed_rotation * abs_rotation;

	if (rotation.y < 0.0f)
	{
		rotation.y += 360.0f;
	}
	else if (rotation.y > 360.0f)
	{
		rotation.y -= 360.0f;
	}

	//���x���
	const float speed_max = 1.0f;
	if (power > speed_max)
	{
		power = speed_max;
	}
	else if (power < -speed_max)
	{
		power = -speed_max;
	}

	//�O�����ɐi��
	velocity = forwordVec;

	position += velocity * speed_move * power;
}

void Player::MovePos_key()
{
	if (!isCanInput)
	{
		return;
	}

	//��]����
	if (Input::DownKey(DIK_RIGHT) && angle < 91.0f)
	{
		angle++;
	}
	if (Input::DownKey(DIK_LEFT) && angle > 89.0f)
	{
		angle--;
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

void Player::MovePos_brake()
{
	const float antiPower = power / 50;//�����x

	//0�ɋ߂Â���
	if (power < -antiPower)
	{
		power += antiPower;
	}
	else if (power > antiPower)
	{
		power -= antiPower;
	}
	else
	{
		power = 0.0f;//���S��~
	}
}
