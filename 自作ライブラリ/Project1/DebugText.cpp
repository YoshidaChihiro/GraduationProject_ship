#include "DebugText.h"

#include <array>
std::array<std::unique_ptr<Sprite>, maxCharCount> DebugText::spriteData = {};
std::vector<Status> DebugText::statues = {};
int  DebugText::spriteIndex = 0;


void DebugText::Initialize()
{
	for (int i = 0; i < spriteData.size(); i++)
	{
		spriteData[i] = nullptr;
	}
	statues.resize(spriteData.size());
}

void DebugText::Print(const std::string & text,const float& posX,const float& posY,const float& scale)
{
	for (int i = 0; i < text.size(); i++)
	{
		if (spriteData[spriteIndex].get() == nullptr)
		{
			spriteData[spriteIndex] = std::make_unique<Sprite>();
		}
		if (spriteIndex >= maxCharCount)
		{
			break;
		}

		const unsigned char& character = text[i];

		int fontIndex = character - 32;
		if (character >= 0x7f)
		{
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;
		Status status;
		status.position = XMFLOAT2(posX + fontWidth * scale*scale * i, posY);
		status.scale = XMFLOAT2{ scale,scale };
		statues[spriteIndex] = status;
		spriteData[spriteIndex]->SpriteSetTextureRect("Debug",fontIndexX*fontWidth, fontIndexY*fontHeight, fontWidth, fontHeight);
		spriteIndex++;
	}

}

void DebugText::Draw(const XMFLOAT4& color)
{
	for (int i = 0; i < spriteIndex; i++)
	{
		spriteData[i]->DrawSprite("Debug", statues[i].position, 0.0f, statues[i].scale, color);
	}
	spriteIndex = 0;
}

void DebugText::End()
{
}
