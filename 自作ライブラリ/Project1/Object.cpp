#include"Object.h"
#include"PtrDelete.h"
#include"BaseCollider.h"
#include"CollisionManager.h"
int Object::bbIndex = 0;

Object::~Object()
{
	if (collider)
	{
		//コリジョンマネージャーから登録を解除する
		CollisionManager::GetInstance()->RemoveCollider(collider);
		PtrDelete(collider);
	}
}

void Object::Create(Model * model)
{
	object.reset(Object3D::Create(model, position, scale, rotation, color));
}

void Object::Initialize()
{
	name = typeid(*this).name();
}

void Object::Update()
{
	if(billboardType != BILLBOARD_TYPE::NONE)
		object->Update(billboardType);
	else
		object->Update();

	if (collider)
		collider->Update();
}

void Object::DrawReady()
{
	if (Object3D::GetDrawShadow())
		pipelineName = "ShadowMap";
	else
	{
		pipelineName = "BasicObj";
		//pipelineName = "NoShade";

	}
}


void Object::Draw()
{
	if (object)
		object->Draw();
}

void Object::CustomDraw(const bool fbx, const bool shade, BLENDTYPE type,const bool customPipeline, const int lightRootParameterIndex )
{
	if (object)
		object->Draw(fbx, shade, type, customPipeline,lightRootParameterIndex);
}

void Object::SetCollider(BaseCollider * collider)
{
	collider->SetObject(this);
	this->collider = collider;
	object->Update();
	collider->Update();
	CollisionManager::GetInstance()->AddCollider(collider);
}
