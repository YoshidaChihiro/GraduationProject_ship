#pragma once
#pragma once
#include"BaseCollider.h"
#include"CollisionPrimitive.h"

class PlaneCollider :public BaseCollider, public Plane
{
private:

public:
	PlaneCollider(const Vector3& max,const Vector3&min):maxPos(max), minPos(min)
	{
		shapeType = COLLISIONSHAPE_PLANE;
	}

	void Update()override ;

	inline const Vector3& GetMax() override { return maxPos; }
	
	inline const Vector3& GetMin() override { return minPos; }
	
	void SetMaxPos(const Vector3& arg_maxPos) { maxPos = arg_maxPos; }
	void SetMinPos(const Vector3& arg_minPos) { minPos = arg_minPos; }

private:
	Vector3 maxPos,minPos;
};


