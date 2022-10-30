#pragma once
#include <vector>

class CourseSquare;
class CourseBuilder
{
public:
	static const float onesize;

	//レインボーロード
	static std::vector<CourseSquare*> BuildCourse_RR();

	//直線
	static std::vector<CourseSquare*> BuildCourse_straight();

};

