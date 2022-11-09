#include "Play.h"
#include "PtrDelete.h"
#include "Object3D.h"
#include "Sprite3D.h"
#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include "Audio.h"
#include "Input.h"
#include "TimerRecord.h"
#include "RankingInGame.h"
#include "SpeedMeter.h"
#include "ResultInGame.h"
#include "GoalCounter.h"
#include "Player.h"
#include "CourseSquare.h"
#include "Ground.h"
#include "CourseObstacle.h"
#include "CourseBuilder.h"
#include "GoalSquare.h"
#include "Arudino.h"

Play::Play()
{
	next = Ending;

	camera = std::make_unique<InGameCamera>();
	Object3D::SetCamera(camera.get());
	Sprite3D::SetCamera(camera.get());

	ParticleEmitter::Initialize();
	ParticleManager::GetInstance()->SetCamera(camera.get());

	lightGroup.reset(LightGroup::Create());
	Object3D::SetLightGroup(lightGroup.get());
	lightGroup->SetDirLightActive(0, true);
	lightGroup->SetDirLightColor(0, { 1,1,1 });

	collisionManager = CollisionManager::GetInstance();

	objectManager = ObjectManager::GetInstance();
	objectManager->AddObjectsAtOnce();
	ParticleEmitter::SetObjectManager(objectManager);

	timer = new TimerRecord();

	rank = new RankingInGame();

	speedMeter = new SpeedMeter();

	resultView = new ResultInGame();

	goalCounter = new GoalCounter();
}


Play::~Play()
{
	ParticleManager::GetInstance()->ClearDeadEffect();
	PtrDelete(timer);
	PtrDelete(rank);
	PtrDelete(speedMeter);
	PtrDelete(resultView);
	PtrDelete(goalCounter);
}

void Play::Initialize()
{
	isEnd = false;
	next = Ending;

	camera->Initialize();
	camera->SetDistance(12.0f);
	camera->SetTheta(0.4f);
	Object3D::SetCamera(camera.get());

	lightGroup->SetAmbientColor({ 1,1,1 });
	lightGroup->SetDirLightDir(0, { 0.0f,-1.0f,0.0f,1 });
	Object3D::SetLightGroup(lightGroup.get());

	isSway = false;

	timer->Initialize();
	timer->Start();

	rank->Initialize();

	speedMeter->Initialize();

	resultView->Initialize();

	goalCounter->Initialize(1);//�����ŃS�[�����邩
	hitGoal_prev = false;

	playerForwordVec_stock = {};

	objectManager->Reset();

	courses_wall.clear();
	grounds.clear();
	courses_obstacle.clear();

	//RR_�X�^�[�g�n�_
	//Vector3 playerPosition = {-10 * CourseBuilder::onesize + (CourseBuilder::onesize / 2), 10, -CourseBuilder::onesize / 2};
	//RR_�S�[���O
	//Vector3 playerPosition = {-10 * CourseBuilder::onesize + (CourseBuilder::onesize / 2), 10, -10 * CourseBuilder::onesize};
	//test_����
	Vector3 playerPosition = { 0, 10, 0 };

	player = new Player(playerPosition);
	objectManager->Add(player);

	//�R�[�X��
	courses_wall = CourseBuilder::BuildCourse_CSV("test.csv");

	//�n��
	Ground* ground = new Ground(Vector3(0, 0, 0), Vector3(24 * CourseBuilder::onesize, 1, 24 * CourseBuilder::onesize));
	objectManager->Add(ground);
	grounds.push_back(ground);

	//�S�[���n�_
	goal = new GoalSquare(
		Vector3(-10 * CourseBuilder::onesize + (CourseBuilder::onesize / 2), 1, -3 * CourseBuilder::onesize),
		Vector3(3 * CourseBuilder::onesize, CourseBuilder::onesize, CourseBuilder::onesize));
	objectManager->Add(goal);

	//��Q��
	CourseObstacle* obstacle = new CourseObstacle({ -10 * CourseBuilder::onesize + (CourseBuilder::onesize / 2),1,2 * CourseBuilder::onesize }, { 3,3,3 });
	objectManager->Add(obstacle);
	courses_obstacle.push_back(obstacle);

	ParticleManager::GetInstance()->ClearDeadEffect();
}

void Play::Update()
{
	//////////////////////////////
	const float default_range = 5300.0f;//�����̒l(!!�v����!!)
	const int data_R = default_range - Arudino::GetData_ultrasonic(0);
	const int data_L = default_range - Arudino::GetData_ultrasonic(1);

	//���̋���
	float power = (data_R + data_L) / 2;//2�̒l�̕���
	power /= default_range;//0�`1��
	player->SetPower(power);

	//���̌���
	float angle = data_L - data_R;//2�̒l�̍�
	angle /= default_range;//-1�`0�`1��
	angle *= 90;//0�`180��
	angle += 90;
	player->SetAngle(angle);
	//////////////////////////////


#ifdef _DEBUG
	//������
	if (Input::TriggerKey(DIK_R) ||
		Arudino::GetData_microSwitch_Trigger())
	{
		Initialize();
		return;
	}
#endif

	//�S�[��
	const bool hitGoal = PlayerHitGoal();
	if (hitGoal && !hitGoal_prev)
	{
		goalCounter->Add();
	}
	if (goalCounter->GetEnd())
	{
		timer->Goal();
		resultView->SetIsActive(true);
		player->SetIsCanInput(false);
	}
	hitGoal_prev = hitGoal;

	//����\��Ԃ�
	if (timer->GetIsAction())
	{
		player->SetIsCanInput(true);
	}

	//�v���C���[�̐ڒn����
	bool onGround = PlayerOnGround();
	player->SetOnGround(onGround);
	//�v���C���[�̏��H�O����
	bool isCourseOut = CourseOut();
	player->SetIsCourseOut(isCourseOut);

	//��Q������
	bool isHitObstacle = PlayerHitObstacle();
	if (isHitObstacle || isCourseOut)
	{
		//���˕Ԃ�
		player->HitObstacle();
		playerForwordVec_stock = player->GetForwordVec();
	}


	//�^�C�}�[
	timer->Update();

	//�����L���O
	rank->Update(1);

	//���x
	speedMeter->Update(player->GetPower() * 100);

	//���U���g�\��
	resultView->Update();
	if (resultView->GetIsMode())
	{
		next = ModeSelect;
		Audio::AllStopSE();
		ShutDown();
		return;
	}
	else if (resultView->GetIsRetry())
	{
		Initialize();
		Audio::AllStopSE();
		return;
	}
	else if (resultView->GetIsTitle())
	{
		next = Title;
		Audio::AllStopSE();
		ShutDown();
		return;
	}


	//�J����
	if (!player->GetIsHitObstacle())
	{
		camera->SetPhi(DirectX::XMConvertToRadians(-(player->GetRotation().y + 90.0f)));
		camera->SetTarget(player->GetPosition() + (player->GetForwordVec() * 4.0f));
	}
	else
	{
		camera->SetTarget(player->GetPosition() + (playerForwordVec_stock * 4.0f));
	}
	camera->Update();

	lightGroup->Update();
	objectManager->Update();
	collisionManager->CheckAllCollisions();

	ParticleManager::GetInstance()->UpdateDeadEffect();
}

void Play::PreDraw()
{
	rank->Draw();
	speedMeter->Draw();

	objectManager->DrawReady();

	objectManager->PreDraw();
	ParticleManager::GetInstance()->DrawDeadEffect();
}

void Play::PostDraw()
{
	objectManager->PostDraw();
	if (!Object3D::GetDrawShadow())
	{
		DirectXLib::GetInstance()->DepthClear();
	}

	timer->Draw();
	resultView->PostDraw();
}

bool Play::PlayerOnGround()
{
	const Vector3 poition_player = player->GetHitBox().GetPosition();
	const Vector3 scale_player = player->GetHitBox().GetScale();

	bool onGround = false;

	for (int i = 0; i < grounds.size(); i++)
	{
		const Vector3 poition_ground = grounds[i]->GetHitBox().GetPosition();
		const Vector3 scale_ground = grounds[i]->GetHitBox().GetScale();


		float aXR = poition_player.x + (scale_player.x / 2.0f);//A�̉E
		float aXL = poition_player.x - (scale_player.x / 2.0f);//A�̍�
		float aYU = poition_player.y + (scale_player.y / 2.0f);//A�̏�
		float aYD = poition_player.y - (scale_player.y / 2.0f);//A�̉�
		float aZF = poition_player.z - (scale_player.z / 2.0f);//A�̑O
		float aZB = poition_player.z + (scale_player.z / 2.0f);//A�̉�

		float bXR = poition_ground.x + (scale_ground.x / 2.0f);//B�̉E
		float bXL = poition_ground.x - (scale_ground.x / 2.0f);//B�̍�
		float bYU = poition_ground.y + (scale_ground.y / 2.0f);//B�̏�
		float bYD = poition_ground.y - (scale_ground.y / 2.0f);//B�̉�
		float bZF = poition_ground.z - (scale_ground.z / 2.0f);//B�̑O
		float bZB = poition_ground.z + (scale_ground.z / 2.0f);//B�̉�

		onGround = onGround ||
			(aXR > bXL && aXL < bXR &&
				aYU > bYD && aYD < bYU &&
				aZF < bZB && aZB > bZF);
	}

	return onGround;
}

bool Play::CourseOut()
{
	const Vector3 poition_player = player->GetHitBox().GetPosition();
	const Vector3 scale_player = player->GetHitBox().GetScale();

	bool courseOut = false;

	for (int i = 0; i < courses_wall.size(); i++)
	{
		const Vector3 poition_course = courses_wall[i]->GetHitBox().GetPosition();
		const Vector3 scale_course = courses_wall[i]->GetHitBox().GetScale();


		float aXR = poition_player.x + (scale_player.x / 2.0f);//A�̉E
		float aXL = poition_player.x - (scale_player.x / 2.0f);//A�̍�
		float aYU = poition_player.y + (scale_player.y / 2.0f);//A�̏�
		float aYD = poition_player.y - (scale_player.y / 2.0f);//A�̉�
		float aZF = poition_player.z - (scale_player.z / 2.0f);//A�̑O
		float aZB = poition_player.z + (scale_player.z / 2.0f);//A�̉�

		float bXR = poition_course.x + (scale_course.x / 2.0f);//B�̉E
		float bXL = poition_course.x - (scale_course.x / 2.0f);//B�̍�
		float bYU = poition_course.y + (scale_course.y / 2.0f);//B�̏�
		float bYD = poition_course.y - (scale_course.y / 2.0f);//B�̉�
		float bZF = poition_course.z - (scale_course.z / 2.0f);//B�̑O
		float bZB = poition_course.z + (scale_course.z / 2.0f);//B�̉�

		courseOut = courseOut ||
			(aXR > bXL && aXL < bXR &&
				aYU > bYD && aYD < bYU &&
				aZF < bZB && aZB > bZF);
	}

	return courseOut;
}

bool Play::PlayerHitGoal()
{
	const Vector3 poition_player = player->GetHitBox().GetPosition();
	const Vector3 scale_player = player->GetHitBox().GetScale();

	const Vector3 poition_goal = goal->GetHitBox().GetPosition();
	const Vector3 scale_goal = goal->GetHitBox().GetScale();


	float aXR = poition_player.x + (scale_player.x / 2.0f);//A�̉E
	float aXL = poition_player.x - (scale_player.x / 2.0f);//A�̍�
	float aYU = poition_player.y + (scale_player.y / 2.0f);//A�̏�
	float aYD = poition_player.y - (scale_player.y / 2.0f);//A�̉�
	float aZF = poition_player.z - (scale_player.z / 2.0f);//A�̑O
	float aZB = poition_player.z + (scale_player.z / 2.0f);//A�̉�

	float bXR = poition_goal.x + (scale_goal.x / 2.0f);//B�̉E
	float bXL = poition_goal.x - (scale_goal.x / 2.0f);//B�̍�
	float bYU = poition_goal.y + (scale_goal.y / 2.0f);//B�̏�
	float bYD = poition_goal.y - (scale_goal.y / 2.0f);//B�̉�
	float bZF = poition_goal.z - (scale_goal.z / 2.0f);//B�̑O
	float bZB = poition_goal.z + (scale_goal.z / 2.0f);//B�̉�

	bool hitGoal = aXR > bXL && aXL < bXR &&
		aYU > bYD && aYD < bYU &&
		aZF < bZB && aZB > bZF;

	return hitGoal;
}

bool Play::PlayerHitObstacle()
{
	const Vector3 poition_player = player->GetHitBox().GetPosition();
	const Vector3 scale_player = player->GetHitBox().GetScale();

	bool hitObstacle = false;

	for (int i = 0; i < courses_obstacle.size(); i++)
	{
		const Vector3 poition_obstacle = courses_obstacle[i]->GetHitBox().GetPosition();
		const Vector3 scale_obstacle = courses_obstacle[i]->GetHitBox().GetScale();


		float aXR = poition_player.x + (scale_player.x / 2.0f);//A�̉E
		float aXL = poition_player.x - (scale_player.x / 2.0f);//A�̍�
		float aYU = poition_player.y + (scale_player.y / 2.0f);//A�̏�
		float aYD = poition_player.y - (scale_player.y / 2.0f);//A�̉�
		float aZF = poition_player.z - (scale_player.z / 2.0f);//A�̑O
		float aZB = poition_player.z + (scale_player.z / 2.0f);//A�̉�

		float bXR = poition_obstacle.x + (scale_obstacle.x / 2.0f);//B�̉E
		float bXL = poition_obstacle.x - (scale_obstacle.x / 2.0f);//B�̍�
		float bYU = poition_obstacle.y + (scale_obstacle.y / 2.0f);//B�̏�
		float bYD = poition_obstacle.y - (scale_obstacle.y / 2.0f);//B�̉�
		float bZF = poition_obstacle.z - (scale_obstacle.z / 2.0f);//B�̑O
		float bZB = poition_obstacle.z + (scale_obstacle.z / 2.0f);//B�̉�

		hitObstacle = hitObstacle ||
			(aXR > bXL && aXL < bXR &&
				aYU > bYD && aYD < bYU &&
				aZF < bZB && aZB > bZF);
	}

	return hitObstacle;
}
