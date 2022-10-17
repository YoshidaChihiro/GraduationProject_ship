#include "PlaneCollider.h"

void PlaneCollider::Update()
{
	const XMMATRIX& matWorld = object->GetMatWorld();

	Plane::distance = Vector3(matWorld.r[3]).Length();
}

