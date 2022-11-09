#include "SpeedMeter.h"
#include "PtrDelete.h"

SpeedMeter::SpeedMeter()
{
	sprite_base = new Sprite();
	sprite_speed = new NumberSprite(speed);
}

SpeedMeter::~SpeedMeter()
{
	PtrDelete(sprite_base);
	PtrDelete(sprite_speed);
}

void SpeedMeter::Initialize()
{
	speed = 0.0f;
}

void SpeedMeter::Update(const int arg_speed)
{
	speed = arg_speed;
}

void SpeedMeter::Draw()
{
	PipelineState::SetPipeline("Sprite");

	XMFLOAT2 position_base = { 1920 - 300,1080 - 100 };
	XMFLOAT2 position_rank = { position_base.x + 130, position_base.y };

	sprite_speed->Draw(std::to_string((int)speed).size(), "number_default", position_rank);
	sprite_base->DrawSprite("speed_base", position_base, 0.0f, { 0.7f, 0.7f });
}
