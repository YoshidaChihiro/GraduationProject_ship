#pragma once
#include <vector>
#include <string>

class CourseSquare;
class Vector3;
class CourseBuilder
{
public:
	static const float onesize;

	//csv対応
	static std::vector<CourseSquare*> BuildCourse_CSV(const std::string& arg_fileName);

private:
	//CSV読み込み
	static std::vector<std::vector<int>> ReadCSV(const std::string& arg_fileName);

	//「ReadCSV」で読み取った値から壁の座標を算出する
	static void WallPosition_CSV(const std::string& arg_fileName);

	static std::vector<Vector3> positions_wall;
	static std::vector<unsigned int> numbers_wall;
};

