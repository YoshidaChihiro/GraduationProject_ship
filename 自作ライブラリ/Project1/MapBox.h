#pragma once
#include "Object.h"
#include "OBJModel.h"

class MapBox :
	public Object
{
public:
	MapBox(const Vector3& position = {}, const Vector3& scale = { 1,1,1 }, const Vector3& rotation = {});
	void Draw() override;
};

