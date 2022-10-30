#pragma once
#include <vector>

class CourseSquare;
class CourseBuilder
{
public:
	static const float onesize;

	//���C���{�[���[�h
	static std::vector<CourseSquare*> BuildCourse_RR();

	//����
	static std::vector<CourseSquare*> BuildCourse_straight();

};

