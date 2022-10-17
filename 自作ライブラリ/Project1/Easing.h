#pragma once
//https://easings.net/ja
class Easing
{
private:
public:
	static float Lerp(const float& start, const float& end, const float& t);

	static float EaseInSine(const float& start, const float& end, const float& MaxTime, const float& nowCount);
	static float EaseOutSine(const float& start, const float& end, const float& MaxTime, const float& nowCount);
	static float EaseInOutSine(const float& start, const float& end, const float& MaxTime, const float& nowCount);

	static float EaseInQuad(const float& start, const float& end, const float& MaxTime, const float& nowCount);
	static float EaseOutQuad(const float& start, const float& end, const float& MaxTime, const float& nowCount);
	static float EaseInOutQuad(const float& start, const float& end, const float& MaxTime, const float& nowCount);
	
	static float EaseInCubic(const float & start, const float & end,const float& MaxTime, const float& nowCount);
	static float EaseOutCubic(const float & start, const float & end, const float& MaxTime, const float& nowCount);
	static float EaseInOutCubic(const float & start, const float & end, const float& MaxTime, const float& nowCount);

	static float EaseInQuart(const float & start, const float & end, const float& MaxTime, const float& nowCount);
	static float EaseOutQuart(const float & start, const float & end, const float& MaxTime, const float& nowCount);
	static float EaseInOutQuart(const float & start, const float & end, const float& MaxTime, const float& nowCount);

	static float EaseInQuint(const float & start, const float & end, const float& MaxTime, const float& nowCount);
	static float EaseOutQuint(const float & start, const float & end, const float& MaxTime, const float& nowCount);
	static float EaseInOutQuint(const float & start, const float & end, const float& MaxTime, const float& nowCount);

	static float EaseInExpo(const float & start, const float & end, const float& MaxTime, const float& nowCount);
	static float EaseOutExpo(const float & start, const float & end, const float& MaxTime, const float& nowCount);
	static float EaseInOutExpo(const float & start, const float & end, const float& MaxTime, const float& nowCount);

	static float EaseInCirc(const float & start, const float & end, const float& MaxTime, const float& nowCount);
	static float EaseOutCirc(const float & start, const float & end, const float& MaxTime, const float& nowCount);
	static float EaseInOutCirc(const float & start, const float & end, const float& MaxTime, const float& nowCount);

	static float EaseInBack(const float & start, const float & end, const float& MaxTime, const float& nowCount);
	static float EaseOutBack(const float & start, const float & end, const float& MaxTime, const float& nowCount);
	static float EaseInOutBack(const float & start, const float & end, const float& MaxTime, const float& nowCount);

};

