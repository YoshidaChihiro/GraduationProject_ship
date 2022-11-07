#include "CourseBuilder.h"
#include "ObjectManager.h"
#include "CourseSquare.h"
#include <fstream>
#include <sstream>


const float CourseBuilder::onesize = 20.0f;

std::vector<CourseSquare*> CourseBuilder::BuildCourse_CSV(const std::string& arg_fileName)
{
	//CSVファイルから壁の位置情報を読み取る
	const std::vector<Vector3> positions = WallPosition_CSV(arg_fileName);

	ObjectManager* objectManager = ObjectManager::GetInstance();
	std::vector<CourseSquare*> courses_wall;

	for (int i = 0; i < positions.size(); i++)
	{
		const float scale_y = 10.0f;

		CourseSquare* course = new CourseSquare(
			positions[i],
			{ onesize, scale_y, onesize });
		objectManager->Add(course);
		courses_wall.push_back(course);
	}

	return courses_wall;
}

std::vector<std::vector<int>> CourseBuilder::ReadCSV(const std::string& arg_fileName)
{
	std::vector<std::vector<int>> result;

	//ファイルを開く
	std::ifstream file("Resources/CSV/" + arg_fileName);
	if (!file)
	{
		printf("not found CSV\n");
		return result;
	}

	//1行ずつ読み取り
	std::string line;
	while (std::getline(file, line))
	{
		//','ずつ読み取り
		std::vector<int> r;
		std::istringstream stream(line);
		std::string field;
		while (std::getline(stream, field, ','))
		{
			r.push_back(std::stoi(field));
		}

		//
		result.push_back(r);
	}

	file.close();

	return result;
}

std::vector<Vector3> CourseBuilder::WallPosition_CSV(const std::string& arg_fileName)
{
	std::vector<Vector3> result;

	//CSV読み込み
	std::vector<std::vector<int>> csv = ReadCSV(arg_fileName);

	//
	float size_z = csv.size();
	float size_x = 0.0f;
	for (int z = 0; z < size_z; z++)
	{
		size_x = csv[z].size();
		for (int x = 0; x < size_x; x++)
		{
			if (csv[z][x] != 1)
			{
				continue;
			}

			//値が「1」の場合は座標を追加
			const float position_y = 1.0f;
			result.push_back({
				onesize * x,
				position_y,
				onesize * z * -1
			});
		}
	}

	//コースの中心がワールド座標の原点に合うようにずらす
	for (int i = 0; i < result.size(); i++)
	{
		result[i].x += onesize / 2;
		result[i].z -= onesize / 2;

		result[i].x -= (size_x * onesize) / 2;
		result[i].z += (size_z * onesize) / 2;
	}

	return result;
}
