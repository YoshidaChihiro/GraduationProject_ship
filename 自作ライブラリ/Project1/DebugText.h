#pragma once
#include"Texture.h"
#include"PipelineState.h"
#include"Sprite.h"
#include<string>
#include<vector>
#include"window.h"
static const int maxCharCount = 256;//最大文字数
struct Status
{
	XMFLOAT2 position;
	XMFLOAT2 scale;
};
class DebugText
{
public:
	static void Initialize();
	static void Print(const std::string& text,const float& posX,const float& posY,const float& scale = 1.0f);
	static void Draw(const XMFLOAT4& color = {1,1,1,1});
	static void End();
private:
	static const int fontWidth = 9;		//フォント画像内1文字分ぼ横幅
	static const int fontHeight = 18;	//フォント画像内1文字分の縦幅
	static const int fontLineCount = 14;//フォント画像内1行分の文字数
	static std::array<std::unique_ptr<Sprite>,maxCharCount> spriteData;
	static std::vector<Status> statues;
	static int spriteIndex;
};

