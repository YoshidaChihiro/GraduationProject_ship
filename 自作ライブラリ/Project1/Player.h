#pragma once
#include "Object.h"
#include "OBJModel.h"

class Player : public Object
{
public:
	Player(const Vector3& arg_pos);
	~Player();
	void Initialize()override;
	void Update()override;
	void Draw() override;
	void DrawReady() override;

	//�O���̃x�N�g��
	Vector3 GetForwordVec();

	//�ڒn����̌��ʂ�����
	void SetOnGround(const bool arg_onGround);

	//�R�[�X�A�E�g�̌��ʂ�����
	void SetIsCourseOut(const bool arg_isCourseOut);

	//��Q���ɂԂ�������
	void HitObstacle();
	bool GetIsHitObstacle();

	//���͂��󂯕t���邩
	void SetIsCanInput(const bool arg_isCanInput);

	//�f�o�C�X����󂯎�������̊p�x����
	void SetAngle(const float arg_angle_device);
	//�f�o�C�X����󂯎�������̋�������
	void SetPower(const float arg_power_device);

private:
	//�����͂ɂ��ړ�
	void MovePos_sail();
	//�L�[�{�[�h���͂ɂ��ړ�(�f�o�b�O�p)
	void MovePos_key();

	//�����͂̒l�Ɋ񂹂Ă���
	void MovePos_linear();

	//��Q���Ƃ̏Փˎ�
	void MovePos_Obstacle();


	OBJModel* myModel = nullptr;

	Vector3 pos_first = {};
	Vector3 pos_prev = {};
	const float speed_move = 1.0f;
	Vector3 forwordVec = {};

	//�n�ʂƂ̔���
	bool onGround = false;
	Vector3 gravity = {};
	const float gravity_acc = 0.02f;

	//�R�[�X�A�E�g����
	bool isCourseOut = false;

	//��Q���Փ˔���
	bool isHitObstacle = false;
	int count_hitObstacle = 0;
	Vector3 pos_backStep = {};
	Vector3 pos_hitStart = {};
	float rotation_hitStart_y = 0.0f;

	//���͎�t
	bool isCanInput = false;

	//�O���f�o�C�X����̓���
	float angle_device = 0.0f;//���̊p�x
	float power_device = 0.0f;//���̋���

	float angle = 0.0f;//���̊p�x
	float power = 0.0f;//���̋���

};
