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

	//コース外や壁
	//内側
	CourseSquare* course_inside = new CourseSquare(Vector3(0, 1, 0), Vector3(10, 3, 10));
	objectManager->Add(course_inside);
	courses_out.push_back(course_inside);
	//奥側
	CourseSquare* course_upside = new CourseSquare(Vector3(0, 1, 50), Vector3(90, 3, 10));
	objectManager->Add(course_upside);
	courses_out.push_back(course_upside);
	//手前側
	CourseSquare* course_downside = new CourseSquare(Vector3(0, 1, -50), Vector3(90, 3, 10));
	objectManager->Add(course_downside);
	courses_out.push_back(course_downside);
	//右側
	CourseSquare* course_rightside = new CourseSquare(Vector3(50, 1, 0), Vector3(10, 3, 100));
	objectManager->Add(course_rightside);
	courses_out.push_back(course_rightside);
	//左側
	CourseSquare* course_lightside = new CourseSquare(Vector3(-50, 1, 0), Vector3(10, 3, 100));
	objectManager->Add(course_lightside);
	courses_out.push_back(course_lightside);

	//地面
	course_ground = new CourseSquare(Vector3(0, 0, 0), Vector3(100, 1, 100));
	objectManager->Add(course_ground);

	ParticleManager::GetInstance()->ClearDeadEffect();
}

void Play::Update()
{
#ifdef _DEBUG
	//シーン切り替え
	if (Input::TriggerPadButton(XINPUT_GAMEPAD_A) || Input::TriggerKey(DIK_SPACE))
	{
		Audio::AllStopSE();
		ShutDown();
		return;
	}

	//初期化
	if (Input::TriggerKey(DIK_R))
	{
		Initialize();
		return;
	}
#endif

	////////////////////////////////////
	//float power = arudino->ReceiveData();
	//power /= 710.0f;//スライドボリュームの最大値が710
	////風の強さ
	//player->SetPower(power);

	//風の向き
	//player->SetAngle(90.0f);
	////////////////////////////////////

	//プレイヤーの接地判定
	bool onGround = PlayerOnGround();
	player->SetOnGround(onGround);
	//プレイヤーの順路外判定
	bool isCourseOut = CourseOut();
	player->SetIsCourseOut(isCourseOut);

	//衝突
	if (isCourseOut && !isSway)
	{
		//振動
		const int time = 5;
		const float power = 0.1f;
		camera->SetShake(time, power);
		isSway = true;
	}
	if (isSway  && !camera->IsShake())
	{
		isSway = false;
	}

	//カメラ
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


	float aXR = poition_player.x + (scale_player.x / 2.0f);//Aの右
	float aXL = poition_player.x - (scale_player.x / 2.0f);//Aの左
	float aYU = poition_player.y + (scale_player.y / 2.0f);//Aの上
	float aYD = poition_player.y - (scale_player.y / 2.0f);//Aの下
	float aZF = poition_player.z - (scale_player.z / 2.0f);//Aの前
	float aZB = poition_player.z + (scale_player.z / 2.0f);//Aの奥

	float bXR = poition_course.x + (scale_course.x / 2.0f);//Bの右
	float bXL = poition_course.x - (scale_course.x / 2.0f);//Bの左
	float bYU = poition_course.y + (scale_course.y / 2.0f);//Bの上
	float bYD = poition_course.y - (scale_course.y / 2.0f);//Bの下
	float bZF = poition_course.z - (scale_course.z / 2.0f);//Bの前
	float bZB = poition_course.z + (scale_course.z / 2.0f);//Bの奥


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


		float aXR = poition_player.x + (scale_player.x / 2.0f);//Aの右
		float aXL = poition_player.x - (scale_player.x / 2.0f);//Aの左
		float aYU = poition_player.y + (scale_player.y / 2.0f);//Aの上
		float aYD = poition_player.y - (scale_player.y / 2.0f);//Aの下
		float aZF = poition_player.z - (scale_player.z / 2.0f);//Aの前
		float aZB = poition_player.z + (scale_player.z / 2.0f);//Aの奥

		float bXR = poition_course.x + (scale_course.x / 2.0f);//Bの右
		float bXL = poition_course.x - (scale_course.x / 2.0f);//Bの左
		float bYU = poition_course.y + (scale_course.y / 2.0f);//Bの上
		float bYD = poition_course.y - (scale_course.y / 2.0f);//Bの下
		float bZF = poition_course.z - (scale_course.z / 2.0f);//Bの前
		float bZB = poition_course.z + (scale_course.z / 2.0f);//Bの奥

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
