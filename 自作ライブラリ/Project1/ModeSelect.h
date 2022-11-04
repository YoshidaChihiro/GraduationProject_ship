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
		NormalRace,//通常レース
		TimeAttack,//タイムアタック
		DbstacleRace,//障害物避け
		ItemCollect,//アイテム集め
	};
	GAMEMODE mode;
	static const int mode_numMax = 4;

	//選択
	void Mode_mode();
	void Mode_sprites();

	//どのモードのシーンに切り替えるか
	void Chack_nextScene();

	//表示用パネル
	Sprite* panel_sprites[mode_numMax];
	Vector2 panel_scales[mode_numMax];

};
