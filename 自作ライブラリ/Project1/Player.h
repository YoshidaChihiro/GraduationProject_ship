#pragma once
#include "Object.h"
#include "FBXModel.h"

class Player : public Object
{
public:
	Player(const Vector3& arg_pos);
	~Player();
	void Initialize()override;
	void Update()override;
	void Draw() override;
	void DrawReady() override;

	//�f�o�C�X����󂯎�������̊p�x����(0���E�A�����v���)
	void SetAngle(const float arg_angle);
	//�f�o�C�X����󂯎�������̋�������
	void SetPower(const float arg_power);
	//
	bool GetIsSway();

private:
	//�����͂ɂ��ړ�
	void MovePos_mast();
	//�L�[�{�[�h���͂ɂ��ړ�(�f�o�b�O�p)
	void MovePos_key();

	float DegreeToRadian(const float degree);

	FBXModel* myModel = nullptr;

	Vector3 pos_first = {};
	Vector3 pos = {};
	Vector3 rotation = {};
	Vector3 velocity = {};
	const float speed_move = 1.0f;

	//�O���f�o�C�X����̓���
	float angle = 0.0f;//���̊p�x
	float power = 0.0f;//���̋���
	//�O���f�o�C�X�ւ̏o��
	bool isSway = false;//�Ԃ��������ɐU��������

};
