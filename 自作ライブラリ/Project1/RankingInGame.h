#pragma once
#include "NumberSprite.h"

class RankingInGame
{
public:
	RankingInGame();
	~RankingInGame();
	void Initialize();
	void Update(const int arg_rank);
	void Draw();


private:
	Sprite* sprite_base = nullptr;

	NumberSprite* sprite_rank = nullptr;
	float rank = 0.0f;

};
