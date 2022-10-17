#include "MapBox.h"

#include "BoxCollider.h"
#include "CollisionAttribute.h"
#include "DrawMode.h"
#include "OBJLoader.h"

MapBox::MapBox( const Vector3& position, const Vector3& scale, const Vector3& rotation)
{
	this->position = position;
	this->scale = scale;
	this->rotation = rotation;

	name = typeid(*this).name();

	Create(OBJLoader::GetModel("box"));
	//コライダーの追加
	BoxCollider* collider = new BoxCollider;
	collider->SetAttribute(COLLISION_ATTR_LANDSHAPE);
	collider->SetScale(scale/2);
	collider->SetMove(true);
	SetCollider(collider);
}

void MapBox::Draw()
{
#ifdef _DEBUG
	if (DrawMode::GetDrawImGui())
	{
		float rotate[3] = {};
		memcpy(rotate, &rotation, sizeof(float) * 3);
		ImGui::Begin("Box");
		ImGui::SliderFloat3("rotateXYZ", rotate, 0, 360);
		ImGui::End();
		memcpy(&rotation, rotate, sizeof(float) * 3);
		Update();
	}
#endif
	
	Object::Draw();
	//CustomDraw(false, true);
}
