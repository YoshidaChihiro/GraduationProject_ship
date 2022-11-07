#pragma once
#include "NumberSprite.h"

class SpeedMeter
{
public:
	SpeedMeter();
	~SpeedMeter();
	void Initialize();
	void Update(const int arg_speed);
	void Draw();


private:
	Sprite* sprite_base = nullptr;

	NumberSprite* sprite_speed = nullptr;
	float speed = 0.0f;

};
