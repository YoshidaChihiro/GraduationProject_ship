#pragma once
class Alpha
{
public:
	Alpha();
	~Alpha();
	static float GetAlpha();
	static void FeedOut();
	static void FeedIn();
	static void Update();
private:
	static bool feedIn,feedOut;
	static float alpha;
};
static const float FeedTime = 100.0f;

