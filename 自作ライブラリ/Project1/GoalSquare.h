#pragma once
#include "Object.h"
#include "FBXModel.h"

class GoalSquare : public Object
{
public:
	GoalSquare(const Vector3& arg_pos, const Vector3& arg_scale);
	~GoalSquare();
	void Initialize()override;
	void Update()override;
	void Draw() override;
	void DrawReady() override;

private:
	FBXModel* myModel = nullptr;

};
