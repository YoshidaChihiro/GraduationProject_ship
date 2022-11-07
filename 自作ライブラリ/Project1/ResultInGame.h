#pragma once
#include "Sprite.h"

class ResultInGame
{
public:
	ResultInGame();
	~ResultInGame();

	void Initialize();
	void Update();
	void Draw();

	void SetIsActive(const bool arg_isActive);
	bool GetIsMode();
	bool GetIsRetry();
	bool GetIsTitle();

private:
	bool isActive = false;

	Sprite* base = nullptr;
	float alpha_base = 0.0f;

	int select = 0;

	Sprite* frame = nullptr;
	XMFLOAT2 position_frame = {};

	Sprite* select_mode = nullptr;
	Sprite* select_retry = nullptr;
	Sprite* select_title = nullptr;

	const XMFLOAT2 position_mode = {100,900};
	const XMFLOAT2 position_retry = {200,900};
	const XMFLOAT2 position_title = {300,900};

	bool isMode = false;
	bool isRetry = false;
	bool isTitle= false;
};
