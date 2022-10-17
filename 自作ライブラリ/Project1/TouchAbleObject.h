#pragma once
#include"Object.h"
#include"OBJModel.h"
class TouchAbleObject:public Object
{
public:
	void Initialize(OBJModel* model, const Vector3& position = {}, const Vector3& scale = {1,1,1},const Vector3& rotation = {});
};

