#pragma once
#include "Scene.h"
#include "LightGroup.h"
#include "InGameCamera.h"
#include "ObjectManager.h"
#include "CollisionManager.h"

class Arudino;
class Player;
class CourseSquare;

class Play :public Scene
{
public:
	Play();
	~Play() override;
	void Initialize() override;
	void Update() override;
	void PreDraw() override;
	void PostDraw() override;

private:
	bool PlayerOnGround();
	bool CourseOut();

	std::unique_ptr<InGameCamera> camera;
	std::unique_ptr<LightGroup> lightGroup;
	ObjectManager* objectManager = nullptr;
	CollisionManager* collisionManager = nullptr;

	//カメラ振れ
	bool isSway = false;
	int counter_sway = 0;
	void SwayTime();

	//プレイヤー
	Player* player = nullptr;
	//コース外
	std::vector<CourseSquare*> courses_out;
	//コース地面
	CourseSquare* course_ground = nullptr;

	//
	Arudino* arudino = nullptr;
};
