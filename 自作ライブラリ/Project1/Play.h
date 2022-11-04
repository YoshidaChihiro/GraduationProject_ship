#pragma once
#include "Scene.h"
#include "LightGroup.h"
#include "InGameCamera.h"
#include "ObjectManager.h"
#include "CollisionManager.h"

class Arudino;
class Player;
class CourseSquare;
class GoalSquare;
class TimerRecord;
class RankingInGame;

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
	bool PlayerHitGoal();

	std::unique_ptr<InGameCamera> camera;
	std::unique_ptr<LightGroup> lightGroup;
	ObjectManager* objectManager = nullptr;
	CollisionManager* collisionManager = nullptr;

	//カメラ振れ
	bool isSway = false;

	//タイマー
	TimerRecord* timer = nullptr;

	//ランキング
	RankingInGame* rank = nullptr;

	//プレイヤー
	Player* player = nullptr;
	//コース壁
	std::vector<CourseSquare*> courses_wall;
	//コース地面
	std::vector<CourseSquare*> courses_ground;
	//ゴール地点
	GoalSquare* goal = nullptr;

	//
	Arudino* arudino = nullptr;
};
