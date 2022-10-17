#include "NumberSprite.h"

#include "Texture.h"
#include "Vector.h"

NumberSprite::NumberSprite(float& num):num(num)
{
}

void NumberSprite::Draw(const int digits, const std::string& texName, const XMFLOAT2& pos, const XMFLOAT2& scale, const XMFLOAT4& color,const XMFLOAT2& anchorPoint, const std::string& pipelineName)
{
	if (sprites.size() < digits)
	{
		const int add = digits - sprites.size();
		for (int i = 0; i < add; i++)
		{
			std::unique_ptr<Sprite> sprite(new Sprite());
			sprites.push_back(std::move(sprite));
		}
	}
	std::string nuwNum = std::to_string((int)num);
	D3D12_RESOURCE_DESC resDesc = Texture::GetTexBuff(texName).Get()->GetDesc();
	const float width = (float)resDesc.Width / 10; //‰æ‘œ‚Ì‰¡•
	const float height = (float)resDesc.Height; //‰æ‘œ‚Ì‰¡•

	const float offset = (float)(digits - 1) *  anchorPoint.x;
	int k = 0;
	//Œ…‚Ì·
	int zeroClearCount = 0;
	if (nuwNum.size() < digits)
	{
		zeroClearCount = digits - nuwNum.size();
	}
	for (auto i = 0; i < digits; i++)
	{
		int drawNumber = nuwNum[k];
		//¬”“_‚¾‚Á‚½‚ç”ò‚Î‚·
		if(drawNumber == 46)
		{
			k++;
			drawNumber = nuwNum[k];
		}
		//Œ…‚ªƒYƒŒ‚½‚ç0‚Å–„‚ß‚é
		if (zeroClearCount)
		{
			k--;
			zeroClearCount--;
			drawNumber = 48;
		}
		drawNumber -= 48;
		XMFLOAT2 position = pos;
		position.x += i * width * scale.x - offset * width * scale.x;
		sprites[i]->SpriteSetTextureRect(texName, drawNumber * width, 0, width, height);
		sprites[i]->DrawSprite(texName, position, 0, scale, color, { 0.5f,anchorPoint.y }, pipelineName);
		k++;
	}

}

