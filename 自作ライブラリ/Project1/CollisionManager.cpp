#include "CollisionManager.h"
#include"BaseCollider.h"
#include "BoxCollider.h"
#include"Collision.h"
#include "CollisionAttribute.h"
#include "DrawMode.h"
#include "GameSettingParam.h"
#include"MeshCollider.h"
#include "SphereCollider.h"
using namespace DirectX;

CollisionManager * CollisionManager::GetInstance()
{
	static CollisionManager instance;
	return &instance;
}

void CollisionManager::AddCollider(BaseCollider* collider)
{
	colliders[collider->GetObject3D()].push_front(collider);
	Tree::TreeObject<BaseCollider>* p = new Tree::TreeObject<BaseCollider>;
	p->pObject = collider;
	SmartPtr<Tree::TreeObject<BaseCollider>> s;
	s.SetPtr(p);
	const Vector3 min = collider->GetMin();
	const Vector3 max = collider->GetMax();
	spSOFTAry.push_back(s);
	L8Tree.Regist(min, max, s);
}

void CollisionManager::CheckAllCollisions()
{
	auto end_it = spSOFTAry.end();
	for(auto it = spSOFTAry.begin();it != end_it;++it)
	{
		if((*it)->pObject->move)
		{
			(*it)->Remove();
			const Vector3 min = (*it)->pObject->GetMin();
			const Vector3 max = (*it)->pObject->GetMax();
			L8Tree.Regist(min, max, *it);
		}
	}
	
	//const int collisionCount = L8Tree.GetAllCollisionList(ColVect);
	//long c = 0;
	//const auto colCount = collisionCount / 2;	// 2で割るのはペアになっているので
	//for (; c < colCount; c++) {
	//	BaseCollider* colA = ColVect[c * 2];
	//	BaseCollider* colB = ColVect[c * (long)2 + 1];
	//	if (colA->attribute == colB->attribute)
	//		continue;

	//	//どちらも球の場合
	//	if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
	//		colB->GetShapeType() == COLLISIONSHAPE_SPHERE)
	//	{
	//		Sphere* SphereA = dynamic_cast<Sphere*>(colA);
	//		Sphere* SphereB = dynamic_cast<Sphere*>(colB);
	//		DirectX::XMVECTOR inter;
	//		if (Collision::CheckSphere2Sphere(*SphereA, *SphereB, &inter))
	//		{
	//			colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
	//			colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
	//		}
	//	}
	//	//メッシュと球の場合
	//	else if (colA->GetShapeType() == COLLISIONSHAPE_MESH &&
	//		colB->GetShapeType() == COLLISIONSHAPE_SPHERE)
	//	{
	//		MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colA);
	//		Sphere* sphere = dynamic_cast<Sphere*>(colB);
	//		DirectX::XMVECTOR inter;
	//		XMVECTOR tempReject;
	//		if (meshCollider->CheckCollisionSphere(*sphere, &inter,&tempReject)) {
	//			colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
	//			colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter, tempReject));
	//		}
	//	}
	//	//メッシュと球の場合
	//	else if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
	//		colB->GetShapeType() == COLLISIONSHAPE_MESH)
	//	{
	//		MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colB);
	//		Sphere* sphere = dynamic_cast<Sphere*>(colA);
	//		DirectX::XMVECTOR inter;
	//		XMVECTOR tempReject;
	//		if (meshCollider->CheckCollisionSphere(*sphere, &inter,&tempReject)) 
	//		{
	//			colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter, tempReject));
	//			colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
	//		}
	//	}
	//	//ボックス同士の場合
	//	else if (colA->GetShapeType() == COLLISIONSHAPE_BOX &&
	//		colB->GetShapeType() == COLLISIONSHAPE_BOX)
	//	{

	//		
	//		BoxCollider* BoxA = nullptr;
	//		BoxCollider* BoxB = nullptr;
	//		if (colA->GetAttribute() == COLLISION_ATTR_ALLIES)
	//		{
	//			BoxA = dynamic_cast<BoxCollider*>(colA);
	//			BoxB = dynamic_cast<BoxCollider*>(colB);
	//		}
	//		else
	//		{
	//			BoxA = dynamic_cast<BoxCollider*>(colB);
	//			BoxB = dynamic_cast<BoxCollider*>(colA);
	//		}
	//		DirectX::XMVECTOR inter;
	//		XMVECTOR tempReject;

	//		//if (BoxA->CheckRotation()|| BoxB->CheckRotation())
	//		//{
	//			if (Collision::CheckOBBOBB(*BoxA, *BoxB, &inter, &tempReject))
	//			{
	//				colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter, tempReject));
	//				colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter, tempReject));
	//			}
	//		//}
	//		//else if (Collision::CheckAABBAABB(*BoxA, *BoxB, &inter, &tempReject))
	//		//{
	//		//	colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter, tempReject));
	//		//	colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter, tempReject));
	//		//}
	//	}
	//	//ボックスと平面の場合
	//	else if (colA->GetShapeType() == COLLISIONSHAPE_BOX &&
	//		colB->GetShapeType() == COLLISIONSHAPE_PLANE)
	//	{
	//		BoxCollider* BoxA = dynamic_cast<BoxCollider*>(colA);
	//		Plane* plane = dynamic_cast<Plane*>(colB);
	//		DirectX::XMVECTOR inter;
	//		XMVECTOR tempReject;

	//		if (Collision::CheckOBBPlane(*BoxA, *plane, &inter, &tempReject))
	//		{
	//			colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter,tempReject));
	//			colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
	//		}
	//	}
	//	//平面とボックスの場合
	//	else if (colA->GetShapeType() == COLLISIONSHAPE_PLANE &&
	//		colB->GetShapeType() == COLLISIONSHAPE_BOX)
	//	{
	//		BoxCollider* Box = dynamic_cast<BoxCollider*>(colB);
	//		Plane* plane = dynamic_cast<Plane*>(colA);
	//		DirectX::XMVECTOR inter;
	//		XMVECTOR tempReject;

	//		if (Collision::CheckOBBPlane(*Box, *plane, &inter,&tempReject))
	//		{
	//			colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
	//			colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter, tempReject));
	//		}
	//	}

		
		//ボックスと球の場合
		//else if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
		//	colB->GetShapeType() == COLLISIONSHAPE_BOX)
		//{
		//	Sphere* sphere = dynamic_cast<Sphere*>(colA);
		//	BoxCollider* BoxB = dynamic_cast<BoxCollider*>(colB);
		//	DirectX::XMVECTOR inter;
		//	if (Collision::CheckSphereBox(*sphere, *BoxB, &inter))
		//	{
		//		colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
		//		colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
		//	}
		//}
		////ボックスと球の場合
		//else if (colB->GetShapeType() == COLLISIONSHAPE_SPHERE &&
		//	colA->GetShapeType() == COLLISIONSHAPE_BOX)
		//{
		//	Sphere* sphere = dynamic_cast<Sphere*>(colB);
		//	BoxCollider* BoxB = dynamic_cast<BoxCollider*>(colA);
		//	DirectX::XMVECTOR inter;
		//	if (Collision::CheckSphereBox(*sphere, *BoxB, &inter))
		//	{
		//		colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
		//		colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
		//	}
		//}
	//}

	//std::vector<BaseCollider*>::iterator itA;
	//std::vector<BaseCollider*>::iterator itB;

	//itA = colliders.begin();
	//for (; itA != colliders.end(); ++itA)
	//{
	//	itB = itA;
	//	++itB;
	//	for (; itB != colliders.end(); ++itB)
	//	{
	//		BaseCollider* colA = *itA;
	//		BaseCollider* colB = *itB;

	//		//どちらも球の場合
	//		if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
	//			colB->GetShapeType() == COLLISIONSHAPE_SPHERE)
	//		{
	//			Sphere* SphereA = dynamic_cast<Sphere*>(colA);
	//			Sphere* SphereB = dynamic_cast<Sphere*>(colB);
	//			DirectX::XMVECTOR inter;
	//			if (Collision::CheckSphere2Sphere(*SphereA, *SphereB, &inter))
	//			{
	//				colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
	//				colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
	//			}
	//		}
	//		//メッシュと球の場合
	//		else if (colA->GetShapeType() == COLLISIONSHAPE_MESH &&
	//			colB->GetShapeType() == COLLISIONSHAPE_SPHERE) 
	//		{
	//			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colA);
	//			Sphere* sphere = dynamic_cast<Sphere*>(colB);
	//			DirectX::XMVECTOR inter;
	//			if (meshCollider->CheckCollisionSphere(*sphere, &inter)) {
	//				colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
	//				colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
	//			}
	//		}
	//		//メッシュと球の場合
	//		else if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
	//			colB->GetShapeType() == COLLISIONSHAPE_MESH)
	//		{
	//			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colB);
	//			Sphere* sphere = dynamic_cast<Sphere*>(colA);
	//			DirectX::XMVECTOR inter;
	//			if (meshCollider->CheckCollisionSphere(*sphere, &inter))
	//			{
	//				colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
	//				colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
	//			}
	//		}
	//		//ボックス同士の場合(AABB)
	//		else if (colA->GetShapeType() == COLLISIONSHAPE_BOX &&
	//			colB->GetShapeType() == COLLISIONSHAPE_BOX)
	//		{
	//			BoxCollider* BoxA = dynamic_cast<BoxCollider*>(colA);
	//			BoxCollider* BoxB = dynamic_cast<BoxCollider*>(colB);
	//			DirectX::XMVECTOR inter;
	//			if( Collision::CheckAABBAABB(*BoxA, *BoxB, &inter))
	//			{
	//				colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
	//				colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
	//			}
	//		}
	//		//ボックスと球の場合
	//		else if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
	//			colB->GetShapeType() == COLLISIONSHAPE_BOX)
	//		{
	//			Sphere* sphere = dynamic_cast<Sphere*>(colA);
	//			BoxCollider* BoxB = dynamic_cast<BoxCollider*>(colB);
	//			DirectX::XMVECTOR inter;
	//			if (Collision::CheckSphereBox(*sphere, *BoxB, &inter))
	//			{
	//				colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
	//				colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
	//			}
	//		}
	//		//ボックスと球の場合
	//		else if (colB->GetShapeType() == COLLISIONSHAPE_SPHERE &&
	//			colA->GetShapeType() == COLLISIONSHAPE_BOX)
	//		{
	//			Sphere* sphere = dynamic_cast<Sphere*>(colB);
	//			BoxCollider* BoxB = dynamic_cast<BoxCollider*>(colA);
	//			DirectX::XMVECTOR inter;
	//			if (Collision::CheckSphereBox(*sphere, *BoxB, &inter))
	//			{
	//				colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
	//				colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
	//			}
	//		}
	//	}
	//}
}

bool CollisionManager::CheckHitBox(const Box& box, unsigned short attribute)
{
	DWORD collisionCount = L8Tree.GetTargetCollisionList(ColVect, box.minPosition, box.maxPosition);
	DWORD c;
	for (c = 0; c < collisionCount; c++) 
	{
		BaseCollider* col = ColVect[c];
		if (col->attribute != attribute)
			continue;

		//ボックス
		if (col->GetShapeType() == COLLISIONSHAPE_BOX)
		{
			Box* boxB = dynamic_cast<Box*>(col);

			if (Collision::CheckOBBOBB(box, *boxB))
			{
				return true;
			}
		}
	}
	return false;
}

void CollisionManager::Initialize(const Vector3& minLine,const Vector3& maxLine)
{
	if (!L8Tree.Initialize(
		6,
		minLine,
		maxLine))
		assert(0);
	CreateConstBuff();
	CreateVBV();
	viewCollider = false;
}

bool CollisionManager::Raycast(const Ray & ray, BaseCollider* collider, RaycastHit * hitinfo, float maxDistance)
{
	return Raycast(ray,collider, 0xffff, hitinfo, maxDistance);
}

bool CollisionManager::Raycast(const Ray & ray, BaseCollider* collider, unsigned short attribute, RaycastHit * hitinfo, float maxDistance)
{
	bool result = false;
	//走査用のイテレータ
	//std::vector<BaseCollider*>::iterator it;
	//今までで最も近いコライダーを記録する為のイテレータ
	///std::vector<BaseCollider*>::iterator it_hit;
	BaseCollider* hitCol;
	//今までで最も近いコライダーの距離を記録する変数
	float distance = maxDistance;
	//今までで最も近いコライダーとの交点を記録する変数
	XMVECTOR inter;

	DWORD collisionCount = L8Tree.GetTargetCollisionList(ColVect, collider->GetMin(), collider->GetMax());
	DWORD c;
	for (c = 0; c < collisionCount; c++) {
		BaseCollider* colA = ColVect[c];
		//属性が合わなければスキップ
		if (!(colA->attribute & attribute))
			continue;

		//球の場合
		if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE)
		{
			Sphere* sphere = dynamic_cast<Sphere*>(colA);
			float tempDistance;
			XMVECTOR tempinter;
			//当たらなければ除外
			if (!Collision::CheckRay2Sphere(ray, *sphere, &tempDistance, &tempinter))continue;
			//距離が最小でなければ除外
			if (tempDistance >= distance)continue;
			//今までで最も近いのであれば記録を取る
			result = true;
			distance = tempDistance;
			inter = tempinter;
			hitCol = colA;
		}
		//メッシュの場合
		else if (colA->GetShapeType() == COLLISIONSHAPE_MESH)
		{
			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colA);

			float tempDistance;
			DirectX::XMVECTOR templnter;
			if (!meshCollider->CheckCollisionRay(ray, &tempDistance, &templnter))continue;
			if (tempDistance >= distance)continue;

			result = true;
			distance = tempDistance;
			inter = templnter;
			hitCol = colA;
		}
		//OBBの場合
		else if (colA->GetShapeType() == COLLISIONSHAPE_BOX)
		{
			Box* box = dynamic_cast<Box*>(colA);

			float tempDistance;
			XMVECTOR tempinter;
			//当たらなければ除外
			if (!Collision::CheckRay2OBB(ray, *box, &tempDistance, &tempinter))continue;
			//距離が最小でなければ除外
			if (tempDistance >= distance)continue;
			//今までで最も近いのであれば記録を取る
			result = true;
			distance = tempDistance;
			inter = tempinter;
			hitCol = colA;
		}

	}
	//最終的に何かに当たっていたら結果を書き込む
	if (result && hitinfo)
	{
		hitinfo->distance = distance;
		hitinfo->inter = inter;
		hitinfo->collider = hitCol;
		hitinfo->object = hitinfo->collider->GetObject3D();
	}
	return result;
}

void CollisionManager::QuerySphere(const Sphere & sphere, QueryCallback * callback, SphereCollider* collider, unsigned short attribute )
{
	assert(callback);

	std::vector<BaseCollider*>::iterator it;

	DWORD collisionCount = L8Tree.GetTargetCollisionList(ColVect,collider->GetMin(),collider->GetMax());
	DWORD c;
	for (c = 0; c < collisionCount; c++) {
		BaseCollider* col = ColVect[c];
		//属性が合わなければスキップ
		if (!(col->attribute & attribute))
			continue;
		
		//球
		if (col->GetShapeType() == COLLISIONSHAPE_SPHERE)
		{
			Sphere* sphereB = dynamic_cast<Sphere*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			if (!Collision::CheckSphere2Sphere(sphere, *sphereB, &tempInter, &tempReject))
				continue;

			//交差情報をセット
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3D();
			info.inter = tempInter;
			info.reject = tempReject;

			//クエリーコールバック呼び出し
			if (!callback->OnQueryHit(info))
			{
				//戻り値がfalseの場合、継続せずに終了
				return;
			}
		}
		//メッシュ
		if (col->GetShapeType() == COLLISIONSHAPE_MESH)
		{
			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			if (!meshCollider->CheckCollisionSphere(sphere,&tempInter,&tempReject))
				continue;

			//交差情報をセット
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3D();
			info.inter = tempInter;
			info.reject = tempReject;

			//クエリーコールバック呼び出し
			if (!callback->OnQueryHit(info))
			{
				//戻り値がfalseの場合、継続せずに終了
				return;
			}
		}

	}

}

void CollisionManager::QueryBox(const Box& box, QueryCallback* callback, unsigned short attribute1, unsigned short attribute2, BaseCollider* collider)
{
	assert(callback);

	DWORD collisionCount = L8Tree.GetTargetCollisionList(ColVect, box.minPosition, box.maxPosition);
	DWORD c;
	for (c = 0; c < collisionCount; c++) {
		BaseCollider* col = ColVect[c];
		if (col->attribute != attribute1 && col->attribute != attribute2)
			continue;

		//球
		if (col->GetShapeType() == COLLISIONSHAPE_SPHERE)
		{
			Sphere* sphereB = dynamic_cast<Sphere*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			if (!Collision::CheckSphereBox(*sphereB, box, &tempInter, &tempReject))
				continue;

			//交差情報をセット
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3D();
			info.inter = tempInter;
			info.reject = tempReject;

			//クエリーコールバック呼び出し
			if (!callback->OnQueryHit(info))
			{
				//戻り値がfalseの場合、継続せずに終了
				return;
			}
		}
		//ボックス
		if (col->GetShapeType() == COLLISIONSHAPE_BOX)
		{
			Box* boxB = dynamic_cast<Box*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;

			if (!Collision::CheckOBBOBB(box, *boxB, &tempInter, &tempReject))
			{
				continue;
			}
			if (collider != nullptr)
				col->OnCollision(CollisionInfo(collider->GetObject3D(), collider, tempInter, tempReject));
			//交差情報をセット
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3D();
			info.inter = tempInter;
			info.reject = tempReject;
			//if (tempReject.m128_f32[1] < 0)
			//{
			//	static  int a;
			//	a++;
			//}

			//クエリーコールバック呼び出し
			if (!callback->OnQueryHit(info))
			{
				//戻り値がfalseの場合、継続せずに終了
				return;
			}
		}
		//平面
		if (col->GetShapeType() == COLLISIONSHAPE_PLANE)
		{
			Plane* plane = dynamic_cast<Plane*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;

			if (!Collision::CheckOBBPlane(box, *plane, &tempInter, &tempReject))
			{
				continue;
			}
			if (collider != nullptr)
				col->OnCollision(CollisionInfo(collider->GetObject3D(), collider, tempInter, tempReject));
			//交差情報をセット
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3D();
			info.inter = tempInter;
			info.reject = tempReject;
			//if (tempReject.m128_f32[1] < 0)
			//{
			//	static  int a;
			//	a++;
			//}

			//クエリーコールバック呼び出し
			if (!callback->OnQueryHit(info))
			{
				//戻り値がfalseの場合、継続せずに終了
				return;
			}
		}
	}

}

void CollisionManager::DrawCollider()
{
	if(Object3D::GetDrawShadow())
		return;
//#ifdef _DEBUG
//	if (DrawMode::GetDrawImGui())
//	{
//		ImGui::Begin("Collider");
//		ImGui::Checkbox("viewCollider", &viewCollider);
//		ImGui::End();
//	}
//#endif
	
	if (!SettingParam::GetViewCollision())
		return;
	auto cmdlist = DirectXLib::GetInstance()->GetCommandList();
	//定数バッファの送信
	SendConstBuff();
	
	UINT drawBoxNum,drawSphere;
//-----------------------------------BoxColliderの描画--------------------------------------
	SendBoxBuffers(drawBoxNum);
	// パイプラインステートの設定
	PipelineState::SetPipeline("CollisionBox");
	// プリミティブ形状を設定
	//cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	// 頂点バッファの設定
	cmdlist->IASetVertexBuffers(0, 1, &boxVbView);

	// 定数バッファビューをセット
	cmdlist->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	// 描画コマンド
	cmdlist->DrawInstanced(drawBoxNum, 1, 0, 0);
//--------------------------------------------------------------------------------------------

//----------------------------------SphereColliderの描画--------------------------------------
	SendSphereBuffers(drawSphere);
	// パイプラインステートの設定
	PipelineState::SetPipeline("CollisionSphere");
	// プリミティブ形状を設定
	//cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	// 頂点バッファの設定
	cmdlist->IASetVertexBuffers(0, 1, &sphereVbView);

	// 定数バッファビューをセット
	cmdlist->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	// 描画コマンド
	cmdlist->DrawInstanced(drawSphere, 1, 0, 0);
//----------------------------------------------------------------------------------------------
}

void CollisionManager::CreateVBV()
{
	HRESULT result = S_FALSE;
	auto dev = DirectXLib::GetInstance()->GetDevice();
	for (int i = 0; i < 3; i++)
	{
		//-----------------------------ボックス----------------------------------------

			// 頂点バッファ生成
		result = dev->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(sizeof(BoxVBData) * vertexCount),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&boxVertBuff));
		if (FAILED(result)) {
			assert(0);
			return;
		}

		// 頂点バッファビューの作成
		boxVbView.BufferLocation = boxVertBuff->GetGPUVirtualAddress();
		boxVbView.SizeInBytes = sizeof(BoxVBData) * vertexCount;
		boxVbView.StrideInBytes = sizeof(BoxVBData);

		//-----------------------------------------------------------------------------
		//---------------------------------球体----------------------------------------
			// 頂点バッファ生成
		result = dev->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(sizeof(SphereVBData) * vertexCount),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&sphereVertBuff));
		if (FAILED(result)) {
			assert(0);
			return;
		}

		// 頂点バッファビューの作成
		sphereVbView.BufferLocation = sphereVertBuff->GetGPUVirtualAddress();
		sphereVbView.SizeInBytes = sizeof(SphereVBData) * vertexCount;
		sphereVbView.StrideInBytes = sizeof(SphereVBData);

		//------------------------------------------------------------------------------
	}
}
void CollisionManager::CreateConstBuff()
{
	HRESULT result;
	for (int i = 0; i < 3; i++)
	{
		result = DirectXLib::GetInstance()->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBuffData) + 0xff) & ~0xff),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuff));
		if (FAILED(result)) {
			assert(0);
		}
	}
}

void CollisionManager::SendBoxBuffers(UINT& num)
{
	// 頂点バッファへデータ転送
	int vertCount = 0;
	SphereVBData* vertMap2 = nullptr;
	//auto result = sphereVertBuff->Map(0, nullptr, (void**)&vertMap2);
	BoxVBData* vertMap = nullptr;
	auto result = boxVertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		for (auto& it : colliders)
		{
			for (auto& colIt : it.second)
			{
				if (colIt->GetShapeType() != COLLISIONSHAPE_BOX)
					continue;
				BoxCollider* boxCollider = dynamic_cast<BoxCollider*>(colIt);
				vertMap->center = boxCollider->GetObjCenter() + boxCollider->GetOffset();
				vertMap->scale = boxCollider->GetScale();
				vertMap->objCenter = boxCollider->GetObjCenter();
				vertMap->rotation = boxCollider->GetObject3D()->GetRotation();
				vertMap->colRotation = boxCollider->GetRotation();

				// 次の頂点へ
				vertMap++;
				//if(vertCount == 1)
				//{
				//	vertMap2->center = XMFLOAT3(boxCollider->center.m128_f32[0], boxCollider->center.m128_f32[1], boxCollider->center.m128_f32[2]);
				//	vertMap2->radius = 0.1f;
				//	vertMap2++;
				//	vertMap2->center = boxCollider->normaDirect[0] * boxCollider->GetScale().x + XMFLOAT3(boxCollider->center.m128_f32[0], boxCollider->center.m128_f32[1], boxCollider->center.m128_f32[2]);
				//	vertMap2->radius = 0.1f;
				//	vertMap2++;
				//	vertMap2->center = boxCollider->normaDirect[1] * boxCollider->GetScale().y + XMFLOAT3(boxCollider->center.m128_f32[0], boxCollider->center.m128_f32[1], boxCollider->center.m128_f32[2]);
				//	vertMap2->radius = 0.1f;
				//	vertMap2++;
				//	vertMap2->center = boxCollider->normaDirect[2] * boxCollider->GetScale().z + XMFLOAT3(boxCollider->center.m128_f32[0], boxCollider->center.m128_f32[1], boxCollider->center.m128_f32[2]);
				//	vertMap2->radius = 0.1f;
				//	vertMap2++;

				//}
				if (++vertCount >= vertexCount) {
					break;
				}
			}
		}
		boxVertBuff->Unmap(0, nullptr);
		
	}
	num = vertCount;
}

void CollisionManager::SendSphereBuffers(UINT& num)
{
	// 頂点バッファへデータ転送
	int vertCount = 0;
	SphereVBData* vertMap = nullptr;
	auto result = sphereVertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		for (auto& it : colliders)
		{
			for (auto& colIt : it.second)
			{
				if (colIt->GetShapeType() != COLLISIONSHAPE_SPHERE)
					continue;
				SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(colIt);
				vertMap->center = XMFLOAT3(sphereCollider->center.m128_f32[0], sphereCollider->center.m128_f32[1], sphereCollider->center.m128_f32[2]);
				vertMap->radius = sphereCollider->GetRadius();
				// 次の頂点へ
				vertMap++;
				if (++vertCount >= vertexCount) {
					break;
				}
			}
		}
		sphereVertBuff->Unmap(0, nullptr);
	}
	num = vertCount;
}

void CollisionManager::SendConstBuff()
{
	// 定数バッファへデータ転送
	ConstBuffData* constMap = nullptr;
	auto result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		constMap->mat = Object3D::GetCamera()->GetMatViewProjection();
		constBuff->Unmap(0, nullptr);
	}
}
