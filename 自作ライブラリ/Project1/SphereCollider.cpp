#include "SphereCollider.h"
using namespace DirectX;
void SphereCollider::Update()
{
	const XMMATRIX& matWorld = object->GetMatWorld();

	Sphere::center = matWorld.r[3] + offset;
	Sphere::radius = radius;
}

