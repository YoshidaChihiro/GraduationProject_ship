#pragma once
#include "Scene.h"
#include "InGameCamera.h"
#include "LightGroup.h"
#include "Sprite.h"

class ModeSelect : public Scene
{
public:
	ModeSelect();
	~ModeSelect() override;
	void Initialize() override;
	void Update() override;
	void PreDraw() override;
	void PostDraw() override;

private:
	std::unique_ptr<InGameCamera> camera;
	std::unique_ptr<LightGroup> lightGroup;

	enum GAMEMODE
	{
		NormalRace,//�ʏ탌�[�X
		TimeAttack,//�^�C���A�^�b�N
		DbstacleRace,//��Q������
		ItemCollect,//�A�C�e���W��
	};
	GAMEMODE mode;
	static const int mode_numMax = 4;

	//�I��
	void Mode_mode();
	void Mode_sprites();

	//�ǂ̃��[�h�̃V�[���ɐ؂�ւ��邩
	void Chack_nextScene();

	//�\���p�p�l��
	Sprite* panel_sprites[mode_numMax];
	Vector2 panel_scales[mode_numMax];

};
