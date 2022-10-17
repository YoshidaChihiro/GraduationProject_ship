#include "Alpha.h"

float Alpha::alpha = 1.0f;
bool Alpha::feedIn = false;
bool Alpha::feedOut = false;


Alpha::Alpha()
{
}


Alpha::~Alpha()
{
}

float Alpha::GetAlpha()
{
	if (alpha >= 1.0f)
		alpha = 1.0f;
	else if (alpha <= 0.0f)
		alpha = 0.0f;
	return alpha;
}

void Alpha::FeedOut()
{
	feedOut = true;
}

void Alpha::FeedIn()
{
	feedIn = true;
}

void Alpha::Update()
{
	if (feedIn)
	{
		if (alpha < 1.0f)
		{
			alpha += 1.0f / FeedTime;
		}
		if (alpha >= 1.0f)
		{
			alpha = 1.0f;
			feedIn = false;
		}
	}
	if (feedOut)
	{
		if (alpha > 0.0f)
		{
			alpha -= 1.0f / FeedTime;
		}
		if(alpha <= 0.0f)
		{
			alpha = 0.0f;
			feedOut = false;
		}

	}
}
