#pragma once
#include <array>

#include "BoxCollider.h"
#include "DebugCamera.h"
#include "FBXModel.h"
#include "Object.h"
#include "ObjectManager.h"
#include "Timer.h"
#include "NumberSprite.h"

class StandardEnemy;
class EnergyItem;
class PanelItem;
class PanelCutLocus;

class Player : public Object
{
public:
	Player(const Vector3& arg_pos);
	~Player();
	void Initialize()override;
	void Update()override;
	void Draw() override;
	void DrawReady() override;

private:
	FBXModel* myModel = nullptr;
};

