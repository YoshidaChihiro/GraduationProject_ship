#pragma once
#include <vector>
#include <string>

class CourseSquare;
class Vector3;
class CourseBuilder
{
public:
	static const float onesize;

	//csv�Ή�
	static std::vector<CourseSquare*> BuildCourse_CSV(const std::string& arg_fileName);

private:
	//CSV�ǂݍ���
	static std::vector<std::vector<int>> ReadCSV(const std::string& arg_fileName);

	//�uReadCSV�v�œǂݎ�����l����ǂ̍��W���Z�o����
	static void WallPosition_CSV(const std::string& arg_fileName);

	static std::vector<Vector3> positions_wall;
	static std::vector<unsigned int> numbers_wall;
};

