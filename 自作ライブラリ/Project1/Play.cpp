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
#include "CourseObstacle.h"
#include "CourseBuilder.h"
#include "GoalSquare.h"
#include "Arudino.h"

Play::Play()
{
	next = Title;

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

	//////////////////////////////
	//arudino = new Arudino();
	//arudino->Initialize();
	//////////////////////////////
}


Play::~Play()
{
	ParticleManager::GetInstance()->ClearDeadEffect();
	PtrDelete(timer);
	PtrDelete(rank);
	PtrDelete(speedMeter);
	PtrDelete(resultView);
	PtrDelete(goalCounter);

	//////////////////////////////
	//arudino->End();
	//PtrDelete(arudino);
	//////////////////////////////
}

void Play::Initialize()
{
	isEnd = false;
	next = Title;

	camera->Initialize();
	camera->SetDistance(12.0f);
	camera->SetTheta(0.4f);
	Object3D::SetCamera(camera.get());

	lightGroup->SetAmbientColor({ 1,1,1 });
	lightGroup->SetDirLightDir(0, { 0.3f,-0.8f,0.3f,1 });
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

	//RR_�X�^�[�g�n�_
	Vector3 playerPosition = {-10 * CourseBuilder::onesize + (CourseBuilder::onesize / 2), 10, -CourseBuilder::onesize / 2};
	//RR_�S�[���O
	//Vector3 playerPosition = {-10 * CourseBuilder::onesize + (CourseBuilder::onesize / 2), 10, -6 * CourseBuilder::onesize};
	//test_����
	//Vector3 playerPosition = { 0, 10, 0 };

	player = new Player(playerPosition);
	objectManager->Add(player);

	//�R�[�X��
	courses_wall = CourseBuilder::BuildCourse_CSV("RR.csv");

	//�n��
	CourseSquare* course_straight = new CourseSquare(Vector3(0, 0, 0), Vector3(24 * CourseBuilder::onesize, 1, 24 * CourseBuilder::onesize));
	objectManager->Add(course_straight);
	courses_ground.push_back(course_straight);

	//�S�[���n�_
	goal = new GoalSquare(
		Vector3(-10 * CourseBuilder::onesize + (CourseBuilder::onesize / 2), 1, -3 * CourseBuilder::onesize),
		Vector3(3 * CourseBuilder::onesize, 0.5f, CourseBuilder::onesize));
	objectManager->Add(goal);

	//��Q��
	CourseObstacle* obstacle = new CourseObstacle({ -10 * CourseBuilder::onesize + (CourseBuilder::onesize / 2),1,2 * CourseBuilder::onesize }, { 3,3,3 });
	objectManager->Add(obstacle);
	courses_obstacle.push_back(obstacle);

	ParticleManager::GetInstance()->ClearDeadEffect();
}

void Play::Update()
{
#ifdef _DEBUG
	//������
	if (Input::TriggerKey(DIK_R))
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

	//////////////////////////////
	//arudino->ReceiveData();
	//const float default_range = 2000.0f;//�������̒l(!!�v����!!)
	//const int data_R = default_range - arudino->GetData(0);
	//const int data_L = default_range - arudino->GetData(1);

	////���̋���
	//float power = (data_R + data_L) / 2;//2�̒l�̕���
	//power /= default_range;//0�`1��
	//player->SetPower(power);

	//���̌���
	//int angle = data_L - data_R;//2�̒l�̍�
	//angle /= default_range;//-1�`0�`1��
	//angle *= 90;//0�`180��
	//angle += 90;
	//player->SetAngle(angle);
	//////////////////////////////

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
	//rank->Update(1);

	//���x
	speedMeter->Update(player->GetPower() * 100);

	//���U���g�\��
	resultView->Update();
	/*if (resultView->GetIsMode())
	{
		next = ModeSelect;
		Audio::AllStopSE();
		ShutDown();
		return;
	}
	else */if (resultView->GetIsRetry())
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
	//rank->Draw();
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
	const Vector3 poition_player = player->GetPosition();
	const Vector3 scale_player = player->GetScale();

	bool onGround = false;

	for (int i = 0; i < courses_ground.size(); i++)
	{
		const Vector3 poition_course = courses_ground[i]->GetPosition();
		const Vector3 scale_course = courses_ground[i]->GetScale();


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

		onGround = onGround ||
			(aXR > bXL && aXL < bXR &&
				aYU > bYD && aYD < bYU &&
				aZF < bZB && aZB > bZF);
	}

	return onGround;
}

bool Play::CourseOut()
{
	const Vector3 poition_player = player->GetPosition();
	const Vector3 scale_player = player->GetScale();

	bool courseOut = false;

	for (int i = 0; i < courses_wall.size(); i++)
	{
		const Vector3 poition_course = courses_wall[i]->GetPosition();
		const Vector3 scale_course = courses_wall[i]->GetScale();


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
	const Vector3 poition_player = player->GetPosition();
	const Vector3 scale_player = player->GetScale();

	const Vector3 poition_course = goal->GetPosition();
	const Vector3 scale_course = goal->GetScale();


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

	bool hitGoal = aXR > bXL && aXL < bXR &&
		aYU > bYD && aYD < bYU &&
		aZF < bZB && aZB > bZF;

	return hitGoal;
}

bool Play::PlayerHitObstacle()
{
	const Vector3 poition_player = player->GetPosition();
	const Vector3 scale_player = player->GetScale();

	bool hitObstacle = false;

	for (int i = 0; i < courses_obstacle.size(); i++)
	{
		const Vector3 poition_course = courses_obstacle[i]->GetPosition();
		const Vector3 scale_course = courses_obstacle[i]->GetScale();


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

		hitObstacle = hitObstacle ||
			(aXR > bXL && aXL < bXR &&
				aYU > bYD && aYD < bYU &&
				aZF < bZB && aZB > bZF);
	}

	return hitObstacle;
}
