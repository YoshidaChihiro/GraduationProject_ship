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

	//�J�����U��
	bool isSway = false;

	//�^�C�}�[
	TimerRecord* timer = nullptr;

	//�����L���O
	RankingInGame* rank = nullptr;

	//�v���C���[
	Player* player = nullptr;
	//�R�[�X��
	std::vector<CourseSquare*> courses_wall;
	//�R�[�X�n��
	std::vector<CourseSquare*> courses_ground;
	//�S�[���n�_
	GoalSquare* goal = nullptr;

	//
	Arudino* arudino = nullptr;
};
