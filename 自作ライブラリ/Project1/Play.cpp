#include "Play.h"
#include "Object3D.h"
#include "Sprite3D.h"
#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include "Audio.h"
#include "Input.h"
#include "Player.h"
#include "CourseSquare.h"
#include "PtrDelete.h"
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

	//arudino = new Arudino();
	//arudino->Initialize();
}


Play::~Play()
{
	ParticleManager::GetInstance()->ClearDeadEffect();
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
	lightGroup->SetDirLightDir(0, { 0.2f,-0.5f,0.7f,1 });
	Object3D::SetLightGroup(lightGroup.get());

	isSway = false;
	counter_sway = 0;

	objectManager->Reset();

	player = new Player(Vector3(25, 10, -10));
	objectManager->Add(player);

	//�R�[�X�O���
	//����
	CourseSquare* course_inside = new CourseSquare(Vector3(0, 1, 0), Vector3(10, 3, 10));
	objectManager->Add(course_inside);
	courses_out.push_back(course_inside);
	//����
	CourseSquare* course_upside = new CourseSquare(Vector3(0, 1, 50), Vector3(90, 3, 10));
	objectManager->Add(course_upside);
	courses_out.push_back(course_upside);
	//��O��
	CourseSquare* course_downside = new CourseSquare(Vector3(0, 1, -50), Vector3(90, 3, 10));
	objectManager->Add(course_downside);
	courses_out.push_back(course_downside);
	//�E��
	CourseSquare* course_rightside = new CourseSquare(Vector3(50, 1, 0), Vector3(10, 3, 100));
	objectManager->Add(course_rightside);
	courses_out.push_back(course_rightside);
	//����
	CourseSquare* course_lightside = new CourseSquare(Vector3(-50, 1, 0), Vector3(10, 3, 100));
	objectManager->Add(course_lightside);
	courses_out.push_back(course_lightside);

	//�n��
	course_ground = new CourseSquare(Vector3(0, 0, 0), Vector3(100, 1, 100));
	objectManager->Add(course_ground);

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

	////////////////////////////////////
	//float power = arudino->ReceiveData();
	//power /= 710.0f;//�X���C�h�{�����[���̍ő�l��710
	////���̋���
	//player->SetPower(power);

	//���̌���
	//player->SetAngle(90.0f);
	////////////////////////////////////

	//�v���C���[�̐ڒn����
	bool onGround = PlayerOnGround();
	player->SetOnGround(onGround);
	//�v���C���[�̏��H�O����
	bool isCourseOut = CourseOut();
	player->SetIsCourseOut(isCourseOut);

	//�Փ�
	if (isCourseOut && !isSway)
	{
		//�U��
		const int time = 5;
		const float power = 0.1f;
		camera->SetShake(time, power);
		isSway = true;
	}
	if (isSway  && !camera->IsShake())
	{
		isSway = false;
	}

	//�J����
	camera->SetPhi(-(player->GetRotation().y + 90.0f) * (3.14f / 180.0f));
	camera->SetTarget(player->GetPosition() + (player->GetForwordVec() * 4.0f));
	camera->Update();

	lightGroup->Update();
	objectManager->Update();
	collisionManager->CheckAllCollisions();

	ParticleManager::GetInstance()->UpdateDeadEffect();
}

void Play::PreDraw()
{
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

	const Vector3 poition_course = course_ground->GetPosition();
	const Vector3 scale_course = course_ground->GetScale();


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


	bool onGround = aXR > bXL && aXL < bXR&&
		aYU > bYD && aYD < bYU&&
		aZF < bZB&& aZB > bZF;

	return onGround;
}

bool Play::CourseOut()
{
	const Vector3 poition_player = player->GetPosition();
	const Vector3 scale_player = player->GetScale();

	bool courseOut = false;

	for (int i = 0; i < courses_out.size(); i++)
	{
		const Vector3 poition_course = courses_out[i]->GetPosition();
		const Vector3 scale_course = courses_out[i]->GetScale();


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
			aZF < bZB&& aZB > bZF);
	}

	return courseOut;
}

void Play::SwayTime()
{
}
