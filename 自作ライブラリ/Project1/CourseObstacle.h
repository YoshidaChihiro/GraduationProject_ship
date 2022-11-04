#pragma once
#include "Object.h"
#include "OBJModel.h"

class CourseObstacle : public Object
{
public:
	CourseObstacle(const Vector3& arg_pos, const Vector3& arg_scale);
	~CourseObstacle();
	void Initialize()override;
	void Update()override;
	void Draw() override;
	void DrawReady() override;

private:
	OBJModel* myModel = nullptr;

};
