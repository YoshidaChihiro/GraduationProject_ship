#pragma once
#include "Object.h"
#include "OBJModel.h"

class CourseSquare : public Object
{
public:
	CourseSquare(const int arg_csvNum, const Vector3& arg_pos, const Vector3& arg_scale_hitBox);
	~CourseSquare();
	void Initialize()override;
	void Update()override;
	void Draw() override;
	void DrawReady() override;

private:
	//CSV‚Ì”’l‚²‚Æ‚Éƒ‚ƒfƒ‹‚ğ•ÏX
	void ModelChange_csv(const int arg_csvNum);

	OBJModel* myModel = nullptr;

	std::string modelName = "";
};
