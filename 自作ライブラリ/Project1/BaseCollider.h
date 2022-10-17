#pragma once

#include"CollisionTypes.h"
#include"Object.h"
#include"CollisionInfo.h"
class BaseCollider
{
public:
	friend class CollisionManager;

	BaseCollider() = default;
	virtual ~BaseCollider() = default;

	inline void SetObject(Object* object)
	{
		this->object = object;
	}

	inline Object* GetObject3D()
	{
		return object;
	}

	virtual void Update() = 0;

	virtual inline const Vector3& GetMin() = 0;
	virtual inline const Vector3& GetMax() = 0;


	inline CollisionShapeType GetShapeType()
	{
		return shapeType;
	}
	inline void OnCollision(const CollisionInfo& info)
	{
		object->OnCollision(info);
	}

	inline void SetAttribute(unsigned short attribute)
	{
		this->attribute = attribute;
	}

	inline void AddAttribute(unsigned short atrribute)
	{
		this->attribute |= atrribute;
	}

	inline void RemoveAttribute(unsigned short attribute)
	{
		this->attribute &= !attribute;
	}

	inline unsigned short GetAttribute()
	{
		return attribute;
	}

	inline void SetMove(const bool flag)
	{
		move = flag;
	}

protected:
	Object* object = nullptr;
	CollisionShapeType shapeType = SHAPE_UNKNOWN;
	//“–‚½‚è”»’è‘®«
	unsigned short attribute = 0b1111111111111111;
	bool move = false;
};