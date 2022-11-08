#pragma once
#include "Object.h"
#include "OBJModel.h"

class CourseSquare : public Object
{
public:
	CourseSquare(const Vector3& arg_pos, const Vector3& arg_scale_hitBox);
	~CourseSquare();
	void Initialize()override;
	void Update()override;
	void Draw() override;
	void DrawReady() override;

private:
	OBJModel* myModel = nullptr;

};
