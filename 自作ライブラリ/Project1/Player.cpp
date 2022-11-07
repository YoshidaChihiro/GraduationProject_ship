#include "Player.h"
#include "OBJLoader.h"
#include "Input.h"
#include "DrawMode.h"
#include "imgui.h"
#include "CollisionManager.h"
#include "Easing.h"

Player::Player(const Vector3& arg_pos)
{
	//�A�j���[�V�����p�Ƀ��f���̃|�C���^���i�[
	myModel = OBJLoader::GetModel("ship");
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

	isHitObstacle = false;
	count_hitObstacle = 0;
	count_hitObstacle_rotZ = 0;
	pos_backStep = {};
	pos_hitStart = {};
	rotation_hitStart_y = 0.0f;
	rotation_hitStart_z = 0.0f;
	rotEnd_Z = 0;
	count_hitObstacle_rotEndZ = 0;

	isCanInput = false;

	angle_device = 90.0f;
	power_device = 0.0f;
	angle = 90.0f;
	power = 0.0f;
}

void Player::Update()
{
	//�����Ԃ��p�ɑO�t���[���̍��W���i�[
	pos_prev = position;

	//���͂��󂯕t���Ȃ�
	if (!isCanInput)
	{
		power_device = 0.0f;
		//angle_device = 90.0f;
	}

	//�ړ�
	MovePos_sail();
	MovePos_linear();
#ifdef _DEBUG
	MovePos_key();
#endif
	MovePos_Obstacle();

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
	Object::CustomDraw(false, true);
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
		ImGui::DragFloat("angle_device", &angle_device, 1.0f, 0.0f, 180.0f);
		ImGui::DragFloat("power_device", &power_device, 0.01f, -1.0f, 1.0f);
		ImGui::Text("angle : %f\n", angle);
		ImGui::Text("power : %f\n", power);
		ImGui::End();
	}
#endif

	pipelineName = "BasicObj";
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

void Player::HitObstacle()
{
	isHitObstacle = true;
	pos_backStep = position + -velocity * 10.0f;
	pos_hitStart = position;
	rotation_hitStart_y = rotation.y;
	rotation_hitStart_z = rotation.z;
	rotEnd_Z = 35;
	count_hitObstacle_rotEndZ = 0;
}

bool Player::GetIsHitObstacle()
{
	return isHitObstacle;
}

void Player::SetIsCanInput(const bool arg_isCanInput)
{
	isCanInput = arg_isCanInput;
}

void Player::SetAngle(const float arg_angle_device)
{
	angle_device = arg_angle_device;
}

void Player::SetPower(const float arg_power_device)
{
	power_device = arg_power_device;
}

float Player::GetPower()
{
	return power;
}

void Player::MovePos_sail()
{
	//��]������
	const float speed_rotation = (angle - 90.0f) / 100.0f;
	rotation.y += power * speed_rotation;

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
	if (Input::DownKey(DIK_RIGHT) && angle_device < 180.0f)
	{
		angle_device++;
	}
	if (Input::DownKey(DIK_LEFT) && angle_device > 0.0f)
	{
		angle_device--;
	}

	//���x����
	const float power_key = 0.01f;//�����x
	if (Input::DownKey(DIK_UP) && power_device < 1.0f)
	{
		power_device += power_key;
	}
	if (Input::DownKey(DIK_DOWN) && power_device > -1.0f)
	{
		power_device -= power_key;
	}
}

void Player::MovePos_linear()
{
	//���̊p�x
	angle = angle_device;


	//���̋����𔽉f
	const float add = 0.01f;
	if (power < power_device)
	{
		if (power_device - power < add)
		{
			return;
		}
		power += add;
	}
	else if (power > power_device)
	{
		if (power - power_device < add)
		{
			return;
		}
		power -= add;
	}
}

void Player::MovePos_Obstacle()
{
	if (!isHitObstacle)
	{
		return;
	}
	//�J�E���g
	const int limit_obstacle = 60;
	if (count_hitObstacle > limit_obstacle)
	{
		isHitObstacle = false;
		count_hitObstacle = 0;
		isCanInput = true;
		power = 0.0f;
		rotation.y = rotation_hitStart_y;
		rotation.z = 0.0f;
		return;
	}
	const int rotNum_z = 5;//Z���̗h���
	if (count_hitObstacle_rotZ > limit_obstacle / rotNum_z)
	{
		count_hitObstacle_rotZ = 0;
		rotation_hitStart_z = rotation.z;
		count_hitObstacle_rotEndZ++;
		rotEnd_Z *= -1;
		if (count_hitObstacle_rotEndZ == rotNum_z - 1)
		{
			rotEnd_Z = 0.0f;
		}
	}

	isCanInput = false;

	//�X���b�v
	rotation.y = Easing::EaseOutCirc(rotation_hitStart_y, rotation_hitStart_y + 360.0f, limit_obstacle, count_hitObstacle);
	rotation.z = Easing::EaseOutCirc(rotation_hitStart_z, rotEnd_Z, limit_obstacle / rotNum_z, count_hitObstacle_rotZ);

	//�o�b�N�X�e�b�v
	position.x = Easing::EaseOutCirc(pos_hitStart.x, pos_backStep.x, limit_obstacle, count_hitObstacle);
	position.y = Easing::EaseOutCirc(pos_hitStart.y, pos_backStep.y, limit_obstacle, count_hitObstacle);
	position.z = Easing::EaseOutCirc(pos_hitStart.z, pos_backStep.z, limit_obstacle, count_hitObstacle);

	count_hitObstacle++;
	count_hitObstacle_rotZ++;
}
