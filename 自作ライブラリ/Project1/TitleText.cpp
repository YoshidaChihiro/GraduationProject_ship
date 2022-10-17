#include "TitleText.h"

#include "Easing.h"
#include "Input.h"

TitleText::TitleText()
{
	startTex.pos = { 960,650 };
	startTex.color = { 1,0.9f,0.6f,1 };
	endTex.pos = { 960,900 };
	endTex.color = { 1,0.9f,0.6f,1 };
	select = 0;
}

TitleText::~TitleText()
{
}

void TitleText::Initialize()
{
	for (int i = 0; i < ParticleNumber; i++)
	{
		particles[i].color = { 0.0f,0.02f,0.1f,1 };
		particles[i].pos = startTex.pos + Vector2{(float)(std::rand() % 200 - 100), (float)(std::rand() % 100 - 50)};
		const float randX = std::rand() % 200 - 100;
		const float randY = std::rand() % 50 - 25;
		particleStatues[i].basePos = startTex.pos + Vector2{ randX,randY };
		const float randX2 = std::rand() % 100 - 50;
		const float randY2 = std::rand() % 30 - 15;
		particleStatues[i].afterPos = particleStatues[i].basePos + Vector2{randX2, randY2};
		particleStatues[i].accel = { 0.03f,0.06f };
		particleStatues[i].selectMoveVal = 1;
		particleStatues[i].scale = (float)(std::rand() % 70)*0.01f + 0.3f;
	}
	select = 0;
	close = false;
	start = false;
	end = false;
	closeCounter = 0;
	decision = false;
}

bool TitleText::Update()
{
	MoveParticles();
	if (decision)
	{
		closeCounter++;
		if (closeCounter > 90)
			close = true;
		return close;
	}
	
	if (Input::TriggerPadLStickDown() || Input::TriggerPadButton(XINPUT_GAMEPAD_DPAD_DOWN) || Input::TriggerKey(DIK_S))
	{
		if (select < 1)
		{
			select++;
			for (int i = 0; i < ParticleNumber; i++)
			{
				const float randX = std::rand() % 200 - 100;
				const float randY = std::rand() % 50 - 25;
				particleStatues[i].basePos = endTex.pos + Vector2{ randX,randY };
				const float randX2 = std::rand() % 100 - 50;
				const float randY2 = std::rand() % 30 - 15;
				particleStatues[i].afterPos = particleStatues[i].basePos + Vector2{ randX2, randY2 };
				particleStatues[i].selectMoveVal = 3;
			}

		}
	}
	if (Input::TriggerPadLStickUp() || Input::TriggerPadButton(XINPUT_GAMEPAD_DPAD_UP) || Input::TriggerKey(DIK_W))
	{
		if (select > 0)
		{
			select--;
			for (int i = 0; i < ParticleNumber; i++)
			{
				const float randX = std::rand() % 200 - 100;
				const float randY = std::rand() % 50 - 25;
				particleStatues[i].basePos = startTex.pos + Vector2{ randX,randY };
				const float randX2 = std::rand() % 100 - 50;
				const float randY2 = std::rand() % 30 - 15;
				particleStatues[i].afterPos = particleStatues[i].basePos + Vector2{ randX2, randY2 };
				particleStatues[i].selectMoveVal = 3;
			}

		}
	}
	if (Input::TriggerPadButton(XINPUT_GAMEPAD_B) || Input::TriggerKey(DIK_SPACE))
	{
		if(select == 0)
		{
			start = true;
		}
		else
		{
			end = true;
		}
		decision = true;
	}
	return false;
}

void TitleText::Draw()
{

	startTex.spritePtr->DrawSprite("titleStart", startTex.pos, 0, { 1,1 }, startTex.color);
	endTex.spritePtr->DrawSprite("titleEnd", endTex.pos, 0, { 1,1 }, endTex.color);
	for (int i = 0; i < ParticleNumber; i++)
	{
		particles[i].spritePtr->DrawSprite("particle", particles[i].pos, 0, particleStatues[i].scale, particles[i].color, { 0.5f,0.5f }, "NoAlphaToCoverageSprite", ADD);
	}

}

void TitleText::MoveParticles()
{
	for (int i = 0; i < ParticleNumber; i++)
	{
		if(particleStatues[i].selectMoveVal == 1&&decision)
		{
			const XMFLOAT4 col = Vector4{ 0.0f,0.02f,0.1f,1 } * Easing::EaseOutQuad(1, 0, 90, closeCounter);
			particles[i].color = col;
			particleStatues[i].velocity += particleStatues[i].accel;
			particles[i].pos += particleStatues[i].velocity;
			continue;;
		}
		
		if (particleStatues[i].accel.x > 0)
		{
			//位置が特定の位置を過ぎたら
			if (particles[i].pos.x >= particleStatues[i].afterPos.x)
			{
				//加速度反転
				particleStatues[i].accel.x *= -1;
				//ランダムで基本位置からどのくらい離れた位置を特定位置にするか決定
				const float randX = std::rand() % 50;
				particleStatues[i].afterPos.x = particleStatues[i].basePos.x - randX;
			}
		}
		else
		{
			//位置が特定の位置を過ぎたら
			if (particles[i].pos.x <= particleStatues[i].afterPos.x)
			{
				//加速度反転
				particleStatues[i].accel.x *= -1;
				//ランダムで基本位置からどのくらい離れた位置を特定位置にするか決定
				const float randX = std::rand() % 50;
				particleStatues[i].afterPos.x = particleStatues[i].basePos.x + randX;
			}
		}
		if (particleStatues[i].accel.y > 0)
		{
			//位置が特定の位置を過ぎたら
			if (particles[i].pos.y >= particleStatues[i].afterPos.y)
			{
				//加速度反転
				particleStatues[i].accel.y *= -1;
				//ランダムで基本位置からどのくらい離れた位置を特定位置にするか決定
				const float randY = std::rand() % 15;
				particleStatues[i].afterPos.y = particleStatues[i].basePos.y - randY;
			}
		}
		else
		{
			//位置が特定の位置を
			if (particles[i].pos.y <= particleStatues[i].afterPos.y)
			{
				//加速度反転
				particleStatues[i].accel.y *= -1;
				//ランダムで基本位置からどのくらい離れた位置を特定位置にするか決定
				const float randY = std::rand() % 15;
				particleStatues[i].afterPos.y = particleStatues[i].basePos.y + randY;
			}
		}

		particleStatues[i].velocity += particleStatues[i].accel;

		const float MaxVelX = 2.0f;
		//速度の最高値を設定
		if (fabs(particleStatues[i].velocity.x) > MaxVelX)
		{
			if (particleStatues[i].velocity.x > 0)
				particleStatues[i].velocity.x = MaxVelX;
			else
				particleStatues[i].velocity.x = -MaxVelX;
		}
		const float MaxVelY = 1.0f * particleStatues[i].selectMoveVal;
		if (fabs(particleStatues[i].velocity.y) > MaxVelY)
		{
			if (particleStatues[i].velocity.y > 0)
				particleStatues[i].velocity.y = MaxVelY;
			else
				particleStatues[i].velocity.y = -MaxVelY;
		}
		particles[i].pos += particleStatues[i].velocity;

		if(particleStatues[i].selectMoveVal == 3
			&& particles[i].pos.y > particleStatues[i].basePos.y - 40 && particles[i].pos.y < particleStatues[i].basePos.y + 40)
		{
			particleStatues[i].selectMoveVal = 1;
		}
	}
}
