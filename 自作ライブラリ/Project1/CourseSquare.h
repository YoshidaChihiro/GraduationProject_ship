#pragma once
#include "Object.h"
#include "FBXModel.h"

class CourseSquare : public Object
{
public:
	CourseSquare(const Vector3& arg_pos, const Vector3& arg_scale);
	~CourseSquare();
	void Initialize()override;
	void Update()override;
	void Draw() override;
	void DrawReady() override;

private:
	FBXModel* myModel = nullptr;

};
