#include "CourseBuilder.h"
#include "ObjectManager.h"
#include "CourseSquare.h"

const float CourseBuilder::onesize = 20.0f;

std::vector<CourseSquare*> CourseBuilder::BuildCourse_RR()
{
	ObjectManager* objectManager = ObjectManager::GetInstance();
	std::vector<CourseSquare*> courses_wall;

	const Vector3 positions[] = {
		{-12 * onesize + (onesize / 2), 1, 0},
		{-8 * onesize + (onesize / 2), 1, 0},
		{-6 * onesize + (onesize / 2), 1, 0},
		{-2 * onesize + (onesize / 2), 1, 0},
		{3 * onesize - (onesize / 2), 1, 6 * onesize},
		{7 * onesize - (onesize / 2), 1, 6 * onesize},
		{8 * onesize - (onesize / 2), 1, -6 * onesize},
		{12 * onesize - (onesize / 2), 1, -6 * onesize},

		{-3 * onesize + (onesize / 2), 1, 12 * onesize - (onesize / 2)},
		{-3 * onesize + (onesize / 2), 1, 8 * onesize - (onesize / 2)},
		{-2 * onesize + (onesize / 2), 1, 5 * onesize - (onesize / 2)},
		{3 * onesize - (onesize / 2), 1, 1 * onesize - (onesize / 2)},
		{5 * onesize, 1, -1 * onesize + (onesize / 2)},
		{1 * onesize, 1, -5 * onesize + (onesize / 2)},
		{0, 1, -8 * onesize + (onesize / 2)},
		{0, 1, -12 * onesize + (onesize / 2)},
	};
	const float scale_y = 10;
	const Vector3 scales[] = {
		{onesize, scale_y, 24 * onesize},
		{onesize, scale_y, 16 * onesize},
		{onesize, scale_y, 10 * onesize},
		{onesize, scale_y, 2 * onesize},
		{onesize, scale_y, 4 * onesize},
		{onesize, scale_y, 12 * onesize},
		{onesize, scale_y, 4 * onesize},
		{onesize, scale_y, 12 * onesize},

		{17 * onesize, scale_y, onesize},
		{9 * onesize, scale_y, onesize},
		{7 * onesize, scale_y, onesize},
		{7 * onesize, scale_y, onesize},
		{12 * onesize, scale_y, onesize},
		{12 * onesize, scale_y, onesize},
		{14 * onesize, scale_y, onesize},
		{22 * onesize, scale_y, onesize},
	};
	const int blockNum = 16;

	for (int i = 0; i < blockNum; i++)
	{
		CourseSquare* course = new CourseSquare(positions[i], scales[i]);
		objectManager->Add(course);
		courses_wall.push_back(course);
	}


	return courses_wall;
}

std::vector<CourseSquare*> CourseBuilder::BuildCourse_straight()
{
	ObjectManager* objectManager = ObjectManager::GetInstance();
	std::vector<CourseSquare*> courses_wall;

	const Vector3 positions[] = {
		{0, 1, 325},	//âú
		{0, 1, -5},		//éËëO
		{15, 1, 150},	//âE
		{-15, 1, 150},	//ç∂
	};
	const Vector3 scales[] = {
		{30, 3, 50},
		{30, 3, 10},
		{10, 3, 300},
		{10, 3, 300},
	};
	const int blockNum = 4;

	for (int i = 0; i < blockNum; i++)
	{
		CourseSquare* course = new CourseSquare(positions[i], scales[i]);
		objectManager->Add(course);
		courses_wall.push_back(course);
	}

	return courses_wall;
}
