#include "RankingInGame.h"
#include "PtrDelete.h"

RankingInGame::RankingInGame()
{
	sprite_base = new Sprite();
	sprite_rank = new NumberSprite(rank);
}

RankingInGame::~RankingInGame()
{
	PtrDelete(sprite_base);
	PtrDelete(sprite_rank);
}

void RankingInGame::Initialize()
{
	rank = 0.0f;
}

void RankingInGame::Update(const int arg_rank)
{
	rank = arg_rank;
}

void RankingInGame::Draw()
{
	PipelineState::SetPipeline("Sprite");

	XMFLOAT2 position_base = { 1920 - 300,1080 - 250 };
	XMFLOAT2 position_rank = { position_base.x + 150, position_base.y};

	sprite_rank->Draw(1,"number_default", position_rank);
	sprite_base->DrawSprite("rank_base", position_base, 0.0f, {0.7f, 0.7f});
}
