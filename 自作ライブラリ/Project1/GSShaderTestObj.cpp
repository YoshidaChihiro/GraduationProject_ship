#include "GSShaderTestObj.h"

#include "BoxCollider.h"
#include "Collision.h"
#include "CollisionManager.h"
#include "DrawMode.h"
#include"OBJLoader.h"
#include "Texture.h"

std::array<ComPtr<ID3D12Resource>, 3> GSShaderTestObj::constBuff = {}; // 定数バッファ
std::array<ComPtr<ID3D12Resource>, 3> GSShaderTestObj::constCameraBuff = {}; // 定数バッファ

GSShaderTestObj::ConstBuffData GSShaderTestObj::sendData = {0,1.00f,0.00f,0.10f,1,0,0,1,{0,0,0},0.1f,{},1.4f,0};

void GSShaderTestObj::Draw()
{	
	auto cmdList = DirectXLib::GetInstance()->GetCommandList();
	if (!Object3D::GetDrawShadow())
	{
		if (pipelineName == "PolygonBreak")
		{
			cmdList->SetGraphicsRootConstantBufferView(4, constBuff[bbIndex]->GetGPUVirtualAddress());
			cmdList->SetGraphicsRootConstantBufferView(5, constCameraBuff[bbIndex]->GetGPUVirtualAddress());
			cmdList->SetGraphicsRootDescriptorTable(6, Texture::GetGpuDescHandleSRV("shadowMap" + std::to_string(bbIndex)));  //ヒープの先頭が定数バッファ
		}
		else
		{
			Object::Draw();
			return;
		}
	}
	CustomDraw(false, true, ALPHA, true);
}

void GSShaderTestObj::DrawReady()
{
	if (Object3D::GetDrawShadow())
	{
		// パイプラインステートの設定
		pipelineName = "ShadowMap";
	}
	else
	{
		bool breakPipeline = false;
		auto camera = Object3D::GetCamera();
		Vector3 targetPos = camera->GetTarget();
		const Vector3 cameraPos = camera->GetEye();
		const Vector3 targetToCamera = cameraPos - targetPos;
		// パイプラインステートの設定
		auto colliders = CollisionManager::GetInstance()->GetColliders(this);
		if (colliders == nullptr)
		{
			pipelineName = "PolygonBreak";
			return;
		}
		for (auto& it : *colliders)
		{
			auto maxPos = it->GetMax();
			auto minPos = it->GetMin();
			const Vector3 targetToMaxPos = maxPos - targetPos;
			const Vector3 targetToMinPos = minPos - targetPos;
			const float dotMax = Vector3::Dot(Vector3(targetToCamera).Normalize(), Vector3(targetToMaxPos).Normalize());
			const float dotMin = Vector3::Dot(Vector3(targetToCamera).Normalize(), Vector3(targetToMinPos).Normalize());
			if (dotMax < 0 && dotMin < 0)
				continue;
			if (fabs(Vector3::Length(cameraPos - maxPos)) < 10
				|| fabs(Vector3::Length(cameraPos - minPos)) < 10
				|| fabs(Vector3::Length(targetToMaxPos)) < 10
				|| fabs(Vector3::Length(targetToMinPos)) < 10)
			{
				breakPipeline = true;
				break;
			}
			Ray ray;
			ray.dir = Vector3(targetToCamera).Normalize().ConvertXMVECTOR();
			ray.start = targetPos.ConvertXMVECTOR();
			float dis;
			XMVECTOR inter;
			BoxCollider* col = dynamic_cast<BoxCollider*>(it);
			if (!Collision::CheckRay2OBB(ray, *col, &dis, &inter))
				continue;
			if (fabs(Vector3::Length(cameraPos - inter)) < 10
				|| fabs(Vector3::Length(targetPos - inter)) < 10)
			{
				breakPipeline = true;
				break;
			}
		}
		if (breakPipeline)
			pipelineName = "PolygonBreak";
		else
			pipelineName = "DrawShadowOBJ";
	}
}

void GSShaderTestObj::CreateConstBuff()
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
			IID_PPV_ARGS(&constBuff[i]));
		assert(SUCCEEDED(result));

		result = DirectXLib::GetInstance()->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstLightCameraBuff) + 0xff) & ~0xff),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constCameraBuff[i]));
		assert(SUCCEEDED(result));
	}
}

void GSShaderTestObj::SendConstBuff()
{
	// 定数バッファへデータ転送
	ConstBuffData* constMap = nullptr;
	HRESULT result = constBuff[bbIndex]->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));
	constMap->_Destruction = sendData._Destruction;
	constMap->_ScaleFactor = sendData._ScaleFactor;
	constMap->_PositionFactor = sendData._PositionFactor;
	constMap->_RotationFactor = sendData._RotationFactor;
	constMap->_Tessellation = sendData._Tessellation;
	constMap->_OnEasing = sendData._OnEasing;
	constMap->_OnTargetBreak = sendData._OnTargetBreak;
	constMap->BreakPoint = sendData.BreakPoint;
	constMap->BreakRadius = sendData.BreakRadius;
	constMap->_OnCameraBreak = sendData._OnCameraBreak;
	constMap->TargetPos = Object3D::GetCamera()->GetTarget();
	constMap->CapsuleRadius = sendData.CapsuleRadius;
	constMap->_On4Vertex = sendData._On4Vertex;
	constBuff[bbIndex]->Unmap(0, nullptr);

	ConstLightCameraBuff* constMap2 = nullptr;

	result = constCameraBuff[bbIndex]->Map(0, nullptr, (void**)&constMap2);
	assert(SUCCEEDED(result));
	constMap2->cameraPos = Object3D::GetLightCamera()->GetEye();
	constMap2->viewProjection = XMMatrixLookAtLH(Vector3(Object3D::GetLightCamera()->GetEye()).ConvertXMVECTOR(), Vector3(Object3D::GetLightCamera()->GetTarget()).ConvertXMVECTOR(), Vector3(Object3D::GetLightCamera()->GetUp()).ConvertXMVECTOR()) * XMMatrixOrthographicLH(100, 100, 1.0f, 1000.0f);

	constCameraBuff[bbIndex]->Unmap(0, nullptr);

}

void GSShaderTestObj::ChangeSendData()
{
#ifdef _DEBUG

	if (!Object3D::GetDrawShadow() && DrawMode::GetDrawImGui())
	{
		float bp[3];
		memcpy(bp, &sendData.BreakPoint, sizeof(float) * 3);
		ImGui::Begin("Status");
		//ImGui::Checkbox("draw", &draw);
		ImGui::SliderFloat("destruction", &sendData._Destruction, 0, 1.0f);
		ImGui::SliderFloat("scaleFactor", &sendData._ScaleFactor, 0, 1.0f);
		ImGui::SliderFloat("positionFactor", &sendData._PositionFactor, 0, 2.0f);
		ImGui::SliderFloat("rotationFactor", &sendData._RotationFactor, 0, 30.0f);
		ImGui::SliderInt("tessellation", &sendData._Tessellation, 1, 32);
		ImGui::SliderFloat3("breakPoint", bp, -5, 5);
		ImGui::SliderFloat("breakRadius", &sendData.BreakRadius, 0, 2.0f);
		ImGui::SliderFloat("capsuleRadius", &sendData.CapsuleRadius, 0, 100.0f);
		ImGui::SliderInt("onEasing", &sendData._OnEasing,0,1);
		ImGui::SliderInt("onTargetBreak", &sendData._OnTargetBreak, 0, 1);
		ImGui::SliderInt("onCameraBreak", &sendData._OnCameraBreak, 0, 1);
		ImGui::SliderInt("on4Vertex", &sendData._On4Vertex, 0, 1);

		ImGui::End();
		memcpy(&sendData.BreakPoint, bp, sizeof(float) * 3);

	}
#endif

}
