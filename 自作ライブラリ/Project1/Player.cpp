#include "Player.h"
#include "OBJLoader.h"
#include "Input.h"
#include "Audio.h"
#include "DrawMode.h"
#include "imgui.h"
#include "CollisionManager.h"
#include "Easing.h"

Player::Player(const Vector3& arg_pos)
{
	//アニメーション用にモデルのポインタを格納
	myModel = OBJLoader::GetModel("ship");
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
	position = pos_first;
	pos_prev = position;
	rotation = {};
	velocity = {};
	scale = { 1.5f };

	onGround = false;
	gravity = {};

	isHitObstacle = false;

	count_backStep = 0;
	pos_backStep_start = {};
	pos_backStep_end = {};

	count_slipY = 0;
	rotation_slipY_start = 0.0f;
	rotation_slipY_end = 0.0f;

	count_slipZ = 0;
	rotation_slipZ_start = 0.0f;
	rotation_slipZ_end = 0.0f;

	isCanInput = false;

	angle_device = 90.0f;
	power_device = 0.0f;
	angle = 90.0f;
	power = 0.0f;

	hitBox.SetPosition(position);
	hitBox.SetScale(Vector3(1.0f,0.7f,1.5f) * scale);
}

void Player::Update()
{
	//押し返し用に前フレームの座標を格納
	pos_prev = position;

	//入力を受け付けない
	if (!isCanInput)
	{
		power_device = 0.0f;
		//angle_device = 90.0f;
	}

	//移動
	MovePos_sail();
	MovePos_linear();
#ifdef _DEBUG
	MovePos_key();
#endif
	//壁や障害物との衝突時
	HitActions();

	//落下
	if (!onGround)
	{
		gravity.y -= gravity_acc;
		position += gravity;
	}
	else
	{
		gravity = {};
	}

	hitBox.SetPosition(position);
	Object::Update();

	//前方のベクトルを格納
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
		ImGui::Text("angle_device : %f\n", angle_device);
		ImGui::Text("power_device : %f\n", power_device);
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
	//戻す
	if (!onGround && arg_onGround)
	{
		position = pos_prev;
	}

	onGround = arg_onGround;
}

void Player::HitObstacle()
{
	if (isHitObstacle)
	{
		return;
	}

	isHitObstacle = true;

	InitBackStep();
	InitSlipY();
	InitSlipZ();

	Audio::PlaySE("SE_collision", Audio::volume_se * 0.8f);
}

bool Player::GetIsHitObstacle()
{
	return isHitObstacle;
}

void Player::InitBackStep()
{
	count_backStep = 0;
	pos_backStep_start = position;
	pos_backStep_end = position + -velocity * 30.0f;
}

void Player::InitSlipY()
{
	count_slipY = 0;
	rotation_slipY_start = rotation.y;
	rotation_slipY_end = rotation.y + 360.0f;
}

void Player::InitSlipZ()
{
	count_slipZ = 0;
	rotation_slipZ_start = rotation.z;
	rotation_slipZ_end = 35.0f;
	count_slipZ_end = 0;
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
	//回転させる
	const float speed_rotation = (angle - 90.0f) / 50.0f;
	rotation.y += power * speed_rotation;

	if (rotation.y < 0.0f)
	{
		rotation.y += 360.0f;
	}
	else if (rotation.y > 360.0f)
	{
		rotation.y -= 360.0f;
	}

	//前方向に進む
	velocity = forwordVec;

	position += velocity * speed_move * power;
}

void Player::MovePos_key()
{
	//回転操作
	if (Input::DownKey(DIK_RIGHT) && angle_device < 180.0f)
	{
		angle_device++;
	}
	if (Input::DownKey(DIK_LEFT) && angle_device > 0.0f)
	{
		angle_device--;
	}

	//速度操作
	const float power_key = 0.01f;//加速度
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
	//帆の角度
	angle = angle_device;


	//風の強さを反映
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

void Player::HitActions()
{
	if (!isHitObstacle)
	{
		return;
	}
	isCanInput = false;

	//移動と回転
	bool isEnd = true;
	const int count_limit = 60;
	const int count_slipZ = 5;
	isEnd = BackStep(count_limit) && isEnd;
	isEnd = SlipY(count_limit) && isEnd;
	isEnd = SlipZ(count_limit, count_slipZ) && isEnd;

	//全行動の終了時
	if (isEnd)
	{
		isHitObstacle = false;
		isCanInput = true;
		power = 0.0f;
		//
		rotation.y = rotation_slipY_start;
		//
		rotation.z = 0.0f;
	}

}

bool Player::BackStep(const int arg_limit)
{
	//
	if (count_backStep > arg_limit)
	{
		return true;
	}

	//
	position.x = Easing::EaseOutCirc(pos_backStep_start.x, pos_backStep_end.x, arg_limit, count_backStep);
	position.y = Easing::EaseOutCirc(pos_backStep_start.y, pos_backStep_end.y, arg_limit, count_backStep);
	position.z = Easing::EaseOutCirc(pos_backStep_start.z, pos_backStep_end.z, arg_limit, count_backStep);

	count_backStep++;

	return false;
}

bool Player::SlipY(const int arg_limit)
{
	//
	if (count_slipY > arg_limit)
	{
		return true;
	}

	//
	rotation.y = Easing::EaseOutCirc(rotation_slipY_start, rotation_slipY_end, arg_limit, count_slipY);

	count_slipY++;

	return false;
}

bool Player::SlipZ(const int arg_limit, const int arg_countTotal)
{
	//
	if (count_slipZ > arg_limit / arg_countTotal)
	{
		count_slipZ = 0;
		rotation_slipZ_start = rotation.z;
		rotation_slipZ_end *= -1;//逆方向に

		count_slipZ_end++;//揺れの回数をカウント
		if (count_slipZ_end == arg_countTotal - 1)
		{
			rotation_slipZ_end = 0.0f;//最後は0
		}
		else if (count_slipZ_end >= arg_countTotal)
		{
			return true;
		}
	}

	//
	rotation.z = Easing::EaseOutCirc(rotation_slipZ_start, rotation_slipZ_end, arg_limit / arg_countTotal, count_slipZ);

	count_slipZ++;

	return false;
}
