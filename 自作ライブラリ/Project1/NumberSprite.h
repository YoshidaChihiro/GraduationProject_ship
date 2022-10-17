#pragma once
#include "Sprite.h"

class NumberSprite
{
public:
	NumberSprite(float& num);
	void Draw(const int digits,const std::string& texName = "number",const XMFLOAT2& pos = {0,0}, const XMFLOAT2& scale = { 1.0f,1.0f },const XMFLOAT4& color = { 1,1,1,1 },const XMFLOAT2& anchorPoint = {0.5f,0.5f},const std::string& pipelineName = "Sprite");
private:
	float &num;
	std::vector<std::unique_ptr<Sprite>>sprites;
};

