#include "TouchAbleObject.h"
#include"MeshCollider.h"
#include"CollisionAttribute.h"
void TouchAbleObject::Initialize(OBJModel* model, const Vector3& position, const Vector3& scale, const Vector3& rotation)
{
	this->position = position;
	this->scale = scale;
	this->rotation = rotation;

	name = typeid(*this).name();

	Create(model);
	//コライダーの追加
	MeshCollider* collider = new MeshCollider;
	collider->ConstructTriangles(model);
	collider->SetAttribute(COLLISION_ATTR_LANDSHAPE);
	//collider->SetMove(true);
	SetCollider(collider);
}
