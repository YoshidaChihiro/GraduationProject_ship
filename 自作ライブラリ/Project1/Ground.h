#pragma once
#include "Object.h"
#include "OBJModel.h"

class Ground : public Object
{
public:
	Ground(const Vector3& arg_pos, const Vector3& arg_scale);
	~Ground();
	void Initialize()override;
	void Update()override;
	void Draw() override;
	void DrawReady() override;

private:
	OBJModel* myModel = nullptr;

};
