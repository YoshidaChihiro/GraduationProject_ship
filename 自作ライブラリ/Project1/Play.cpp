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
#include "Player.h"
#include "CourseSquare.h"
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

	//arudino = new Arudino();
	//arudino->Initialize();
}


Play::~Play()
{
	ParticleManager::GetInstance()->ClearDeadEffect();
	PtrDelete(timer);
	PtrDelete(rank);
	//arudino->End();
	//PtrDelete(arudino);
}

void Play::Initialize()
{
	isEnd = false;
	next = Ending;

	camera->Initialize();
	camera->SetDistance(12.0f);
	camera->SetTheta(0.5f);
	Object3D::SetCamera(camera.get());

	lightGroup->SetAmbientColor({ 1,1,1 });
	lightGroup->SetDirLightDir(0, { 0.3f,-0.8f,0.3f,1 });
	Object3D::SetLightGroup(lightGroup.get());

	isSway = false;

	timer->Initialize();
	timer->Start();

	rank->Initialize();

	objectManager->Reset();

	player = new Player(Vector3(-10 * CourseBuilder::onesize + (CourseBuilder::onesize / 2), 10, -CourseBuilder::onesize / 2));
	objectManager->Add(player);

	//�R�[�X��
	courses_wall = CourseBuilder::BuildCourse_RR();

	//�n��
	CourseSquare* course_straight = new CourseSquare(Vector3(0, 0, 0), Vector3(24 * CourseBuilder::onesize, 1, 24 * CourseBuilder::onesize));
	objectManager->Add(course_straight);
	courses_ground.push_back(course_straight);

	//�S�[���n�_
	goal = new GoalSquare(
		Vector3(-10 * CourseBuilder::onesize + (CourseBuilder::onesize / 2), 1, -3 * CourseBuilder::onesize),
		Vector3(3 * CourseBuilder::onesize, 0.5f, CourseBuilder::onesize));
	objectManager->Add(goal);

	ParticleManager::GetInstance()->ClearDeadEffect();
}

void Play::Update()
{
#ifdef _DEBUG
	//�V�[���؂�ւ�
	if (Input::TriggerPadButton(XINPUT_GAMEPAD_A) || Input::TriggerKey(DIK_SPACE))
	{
		Audio::AllStopSE();
		ShutDown();
		return;
	}

	//������
	if (Input::TriggerKey(DIK_R))
	{
		Initialize();
		return;
	}
#endif

	//�S�[��
	if (PlayerHitGoal())
	{
		timer->Goal();
		player->SetIsCanInput(false);
	}

	//����\��Ԃ�
	if (timer->GetIsAction())
	{
		player->SetIsCanInput(true);
	}

	////////////////////////////////////
	//float power = arudino->ReceiveData();
	//power /= 710.0f;//�X���C�h�{�����[���̍ő�l��710
	////���̋���
	//player->SetPower(power);

	////���̌���
	//player->SetAngle(90.0f);
	////////////////////////////////////

	//�v���C���[�̐ڒn����
	bool onGround = PlayerOnGround();
	player->SetOnGround(onGround);
	//�v���C���[�̏��H�O����
	bool isCourseOut = CourseOut();
	player->SetIsCourseOut(isCourseOut);

	////�Փ�
	//if (isCourseOut && !isSway)
	//{
	//	//�U��
	//	const int time = 5;
	//	const float power = 0.1f;
	//	camera->SetShake(time, power);
	//	isSway = true;
	//}
	//if (isSway  && !camera->IsShake())
	//{
	//	isSway = false;
	//}

	//�^�C�}�[
	timer->Update();

	//�����L���O
	rank->Update(1);

	//�J����
	camera->SetPhi(DirectX::XMConvertToRadians(-(player->GetRotation().y + 90.0f)));
	camera->SetTarget(player->GetPosition() + (player->GetForwordVec() * 4.0f));
	camera->Update();

	lightGroup->Update();
	objectManager->Update();
	collisionManager->CheckAllCollisions();

	ParticleManager::GetInstance()->UpdateDeadEffect();
}

void Play::PreDraw()
{
	timer->Draw();
	rank->Draw();

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

	bool hitGoal = aXR > bXL && aXL < bXR&&
		aYU > bYD && aYD < bYU&&
		aZF < bZB&& aZB > bZF;

	return hitGoal;
}
