#include "ObjectManager.h"

#include "Collision.h"
#include "CollisionManager.h"
#include "Object.h"
ObjectManager* ObjectManager::GetInstance()
{
	static ObjectManager instance;
    return &instance;
}

void ObjectManager::Add(Object* object, bool preDraw)
{
	addScheduledObjects[preDraw].push_back(object);
}

void ObjectManager::AddExecute(Object* object, bool preDraw)
{
	std::unique_ptr<Object> obj(object);
	objects[preDraw].push_back(std::move(obj));
}

void ObjectManager::Initialize()
{
	for (auto& it : objects)
	{
		for (auto& itr : it.second)
		{
			itr->Initialize();
		}
	}
}

void ObjectManager::Reset()
{
	for (auto& vecObjects : objects)
	{
		for (auto itr = vecObjects.second.begin(); itr != vecObjects.second.end();)
		{
			if (*itr)
			{
				itr = vecObjects.second.erase(itr);
			}
			else
			{
				itr++;
			}
		}
		vecObjects.second.clear();
	}
}

void ObjectManager::Update()
{
	AddObjectsAtOnce();
	RemoveDeadObjects();

	for (auto& it : objects)
	{
		for (auto& itr : it.second)
		{
			if (!itr->IsDead())
			{
				itr->Update();
			}
		}
	}


}

void ObjectManager::Remove(Object* object)
{
	for (auto& itr : objects)
	{
		for (auto it = itr.second.begin(); it != itr.second.end();)
		{
			if ((*it).get() == object)
			{
				it = itr.second.erase(it);
			}
			else
				++it;
		}
	}
}

void ObjectManager::AddObjectsAtOnce()
{
	for (auto& vecObjects : addScheduledObjects)
	{
		for (auto itr = vecObjects.second.begin(); itr != vecObjects.second.end(); itr++)
		{
			if (!(*itr)->IsDead())
			{
				AddExecute(*itr, vecObjects.first);
			}
			else
			{
				delete* itr;
			}
		}
	}
	addScheduledObjects.clear();
}

void ObjectManager::RemoveDeadObjects()
{
	for (auto& vecObjects : objects)
	{
		for (auto itr = vecObjects.second.begin(); itr != vecObjects.second.end();)
		{
			if ((*itr)->IsDead())
			{
				itr = vecObjects.second.erase(itr);
			}
			else
			{
				itr++;
			}
		}
	}
}

void ObjectManager::DrawReady()
{
	//登録されているオブジェクトの使用パイプライン毎に配列に格納
	drawObjects.clear();
	
	for (auto& it : objects)
	{
		for (auto& itr : it.second)
		{
			////視錐台カリングの判定
			//auto colliders = CollisionManager::GetInstance()->GetColliders(itr.get());
			//if (colliders == nullptr || Object3D::GetDrawShadow())
			//{
			//	itr->DrawReady();
			//	drawObjects[it.first][itr->GetPipelineName()].push_back(itr.get());
			//	continue;
			//}
			//bool draw = false;
			//for (auto& colIt : *colliders)
			//{
			//	Box box;
			//	auto max = colIt->GetMax();
			//	auto min = colIt->GetMin();
			//	if(min.x < max.x)
			//		box.minPosition.x = min.x;
			//	else
			//		box.minPosition.x = max.x;
			//	if (min.y < max.y)
			//		box.minPosition.y = min.y;
			//	else
			//		box.minPosition.y = max.y;
			//	if (min.z < max.z)
			//		box.minPosition.z = min.z;
			//	else
			//		box.minPosition.z = max.z;
			//	box.scale = { fabs(max.x - min.x),fabs(max.y - min.y),fabs(max.z - min.z) };
			//	//box.minPosition = min;
			//	Frustum frustum;
			//	frustum.CalculateFrustumPlanes(*Object3D::GetCamera());
			//	if (Collision::Detect(box, frustum))
			//	{
			//		draw = true;
			//		break;
			//	}
			//}
			//if (draw)
			//{
				itr->DrawReady();
				drawObjects[it.first][itr->GetPipelineName()].push_back(itr.get());
			//}

		}
	}
}

void ObjectManager::PreDraw()
{
	for (auto& itr : drawObjects[true])
	{
		//パイプラインごとの配列になっているため配列の先頭時のみパイプラインを設定
		PipelineState::SetPipeline(itr.first);
		for (auto& objectItr : itr.second)
		{
			objectItr->Draw();
		}
	}

}

void ObjectManager::PostDraw()
{
	for (auto& itr : drawObjects[false])
	{
		//パイプラインごとの配列になっているため配列の先頭時のみパイプラインを設定
		PipelineState::SetPipeline(itr.first);
		for (auto& objectItr : itr.second)
		{
			objectItr->Draw();
		}
	}
}

