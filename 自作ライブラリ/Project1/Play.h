#pragma once
#include "Scene.h"
#include "LightGroup.h"
#include "InGameCamera.h"
#include "ObjectManager.h"
#include "CollisionManager.h"

class Player;
class CourseSquare;
class Ground;
class GoalSquare;
class CourseObstacle;
class TimerRecord;
class RankingInGame;
class SpeedMeter;
class ResultInGame;
class GoalCounter;

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
	bool PlayerHitObstacle();

	std::unique_ptr<InGameCamera> camera;
	std::unique_ptr<LightGroup> lightGroup;
	ObjectManager* objectManager = nullptr;
	CollisionManager* collisionManager = nullptr;

	//タイマー
	TimerRecord* timer = nullptr;

	//ランキング
	RankingInGame* rank = nullptr;
	//速度
	SpeedMeter* speedMeter = nullptr;

	//リザルト表示
	ResultInGame* resultView = nullptr;

	//周回計測
	GoalCounter* goalCounter = nullptr;
	bool hitGoal_prev = false;

	//プレイヤー
	Player* player = nullptr;
	//コース壁
	std::vector<CourseSquare*> courses_wall;
	//コース地面
	std::vector<Ground*> grounds;
	//ゴール地点
	GoalSquare* goal = nullptr;
	//障害物
	std::vector<CourseObstacle*> courses_obstacle;
	Vector3 playerForwordVec_stock = {};


	int data_prev_R = 0;
	int data_prev_L = 0;
};
