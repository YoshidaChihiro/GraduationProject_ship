#pragma once
#include <array>

#include "Sprite.h"
#include "Vector.h"

class TitleText
{
public:
	TitleText();
	~TitleText();
	void Initialize();
	bool Update();
	void Draw();
	bool GetStart() { return start; }
	bool GetEnd() { return end; }

private:
	void MoveParticles();
	struct SpriteStruct
	{
		SpriteStruct()
		{
			spritePtr = new Sprite();
			pos = { 0,0 };
			color = { 1, 1, 1, 1 };
		}
		~SpriteStruct()
		{
			delete spritePtr;
			spritePtr = nullptr;
		}
		Sprite* spritePtr;
		Vector2 pos;
		XMFLOAT4 color;
	};
	SpriteStruct startTex;
	SpriteStruct endTex;
	static const int ParticleNumber = 50;
	std::array<SpriteStruct,ParticleNumber> particles;
	struct ParticleStatus
	{
		Vector2 basePos;
		Vector2 afterPos;
		Vector2 velocity;
		Vector2 scale;
		Vector2 accel;
		int selectMoveVal = 1;
	};
	
	std::array<ParticleStatus, ParticleNumber> particleStatues;
	int select;
	//決定時にパーティクルを飛ばすフラグ
	bool decision;
	bool close;
	bool end;
	bool start;
	int closeCounter;
};

