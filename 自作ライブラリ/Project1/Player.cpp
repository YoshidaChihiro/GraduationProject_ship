#include "Player.h"

#include"OBJLoader.h"
#include"SphereCollider.h"
#include"Input.h"
#include"DebugText.h"
#include"CollisionManager.h"
#include"CollisionAttribute.h"
#include "DrawMode.h"
#include "Easing.h"
#include "FBXManager.h"
#include "GameSettingParam.h"
#include "ParticleEmitter.h"
#include "Audio.h"
#include "ParticleEmitter.h"
#include "AreaEffect.h"

Player::Player(const Vector3& arg_pos)
{
	//アニメーション用にモデルのポインタを格納
	myModel = FBXManager::GetModel("player");
	//モデルの生成
	Create(myModel);

	name = typeid(*this).name();

	Initialize();
}

Player::~Player()
{			
}

void Player::Initialize()
{	
}

void Player::Update()
{
	Object::Update();
}

void Player::Draw()
{
	Object::CustomDraw(true);
}

void Player::DrawReady()
{
#ifdef _DEBUG
	//if (!Object3D::GetDrawShadow() && DrawMode::GetDrawImGui())
	//{
	//	XMMATRIX camMatWorld = XMMatrixInverse(nullptr, camera->GetMatView());
	//	Vector3 cameraDirectionZ = Vector3(camMatWorld.r[2].m128_f32[0], 0, camMatWorld.r[2].m128_f32[2]);
	//	cameraDirectionZ.Normalize();
	//	ImGui::Begin("PlayerStatus");
	//	ImGui::Text("CameraDirection : {%f, %f, %f }\n", cameraDirectionZ.x, cameraDirectionZ.y, cameraDirectionZ.z);
	//	ImGui::Text("Direction : {%f, %f, %f }\n", direction.x, direction.y, direction.z);
	//	ImGui::Text("Position : {%f, %f, %f }\n", position.x, position.y, position.z);
	//	ImGui::Text("Rot : {%f, %f, %f }\n", rotation.x, rotation.y, rotation.z);
	//	ImGui::Text("virtualityPlanePosition : {%f,%f,%f}\n", virtualityPlanePosition.x, virtualityPlanePosition.y, virtualityPlanePosition.z);
	//	ImGui::End();
	//}

#endif

	if (Object3D::GetDrawShadow())
	{
		pipelineName = "FBXShadowMap";
	}
	else
	{
		pipelineName = "FBX";
	}
}

