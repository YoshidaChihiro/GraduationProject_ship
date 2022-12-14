#pragma once
#include "Object.h"
#include "OBJModel.h"

class GoalSquare : public Object
{
public:
	GoalSquare(const Vector3& arg_pos, const Vector3& arg_scale_hitBox);
	~GoalSquare();
	void Initialize()override;
	void Update()override;
	void Draw() override;
	void DrawReady() override;

private:
	OBJModel* myModel = nullptr;

};
