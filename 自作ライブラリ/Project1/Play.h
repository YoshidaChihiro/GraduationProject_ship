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

	//�J�����U��
	bool isSway = false;

	//�^�C�}�[
	TimerRecord* timer = nullptr;

	//�����L���O
	RankingInGame* rank = nullptr;
	//���x
	SpeedMeter* speedMeter = nullptr;

	//���U���g�\��
	ResultInGame* resultView = nullptr;

	//����v��
	GoalCounter* goalCounter = nullptr;
	bool hitGoal_prev = false;

	//�v���C���[
	Player* player = nullptr;
	//�R�[�X��
	std::vector<CourseSquare*> courses_wall;
	//�R�[�X�n��
	std::vector<Ground*> grounds;
	//�S�[���n�_
	GoalSquare* goal = nullptr;
	//��Q��
	std::vector<CourseObstacle*> courses_obstacle;
	Vector3 playerForwordVec_stock = {};
};
