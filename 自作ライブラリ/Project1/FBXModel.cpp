#include "FBXModel.h"
#include"DirectXLib.h"
#include "FbxLoader.h"
#include "Object3D.h"
#include "Texture.h"
#include "Quaternion.h"

void FBXModel::CreateBuffers()
{
	auto dev = DirectXLib::GetInstance()->GetDevice();
	HRESULT result;
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataSkin) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffSkin));
	assert(SUCCEEDED(result));
	//頂点データ全体ｎサイズ
	UINT sizeVB =
		static_cast<UINT>(sizeof(VertexPosNormalUvSkin) *
			vertices.size());

	//頂点バッファの生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));
	//頂点バッファへのデータ転送
	VertexPosNormalUvSkin* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result))
	{
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	//頂点バッファビュー(VBV)の作成
	vbView.BufferLocation =
		vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

	//頂点インデックス全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());
	//インデックスバッファ作成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	assert(SUCCEEDED(result));
	//インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result))
	{
		std::copy(indices.begin(), indices.end(), indexMap);
		indexBuff->Unmap(0, nullptr);
	}
	//インデックスバッファビュー(IBV)の作成
	ibView.BufferLocation =
		indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	assert(SUCCEEDED(result));
	for (int i = 0; i < MAX_BONES; i++)
	{
		constMapSkin->bones[i] = XMMatrixIdentity();
	}
	constBuffSkin->Unmap(0, nullptr);

	//アニメーションの初期化
	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);
	isPlay = true;
}

void FBXModel::Draw()
{

	auto cmdList = DirectXLib::GetInstance()->GetCommandList();
	//頂点バッファをセット(VBV)
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	//インデックスバッファをセット
	cmdList->IASetIndexBuffer(&ibView);

	//デスクリプタヒープのセット
	ID3D12DescriptorHeap* ppHeaps[] = { Texture::GetBasicDescHeap().Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	if (!Object3D::GetDrawShadow())
	{
		//シェーダーリソースビューをセット
		cmdList->SetGraphicsRootDescriptorTable(2, Texture::GetGpuDescHandleSRV(texName));
		cmdList->SetGraphicsRootConstantBufferView(4, constBuffSkin->GetGPUVirtualAddress());
	}
	else
		cmdList->SetGraphicsRootConstantBufferView(1, constBuffSkin->GetGPUVirtualAddress());
	//描画コマンド
	cmdList->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);


}

void FBXModel::AddAnimation(const std::string& animationName, const int startFrame, const int endFrame)
{
	AnimationTime time;
	time.startTime.SetTime(0, 0, 0, startFrame, 0, FbxTime::EMode::eFrames60);
	time.endTime.SetTime(0, 0, 0, endFrame, 0, FbxTime::EMode::eFrames60);
	animations[animationName] = time;

}

const FBXModel::AnimationTime& FBXModel::GetAnimation(const std::string& animationName)
{
	return animations[animationName];
}

void FBXModel::AnimationInit()
{
	//0番のアニメーション取得
	FbxAnimStack* animstack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	//アニメーションの名前取得
	const char* animstackname = animstack->GetName();
	//アニメションの時間取得
	FbxTakeInfo* takeinfo = fbxScene->GetTakeInfo(animstackname);

	//開始時間取得
	startTime = takeinfo->mLocalTimeSpan.GetStart();
	//終了時間取得
	endTime = takeinfo->mLocalTimeSpan.GetStop();
	//開始時間に合わせる
	currentTime = startTime;
	//再生中状態にする
	isPlay = true;
}

void FBXModel::SetAnimationFrame(const int startFrame, const int endFrame, const int FrameTime)
{
	startTime.SetTime(0, 0, 0, startFrame, 0, FbxTime::EMode::eFrames60);
	currentTime = startTime;
	endTime.SetTime(0, 0, 0, endFrame, 0, FbxTime::EMode::eFrames60);
	if (startFrame > endFrame)
		frameTime.SetTime(0, 0, 0, -FrameTime, 0, FbxTime::EMode::eFrames60);
	else
		frameTime.SetTime(0, 0, 0, FrameTime, 0, FbxTime::EMode::eFrames60);
	isPlay = true;
}

void FBXModel::SetAnimation(const std::string& animationName, const int FrameTime)
{
	isPlay = true;

	//登録されていないアニメーション名の場合は0フレーム目を設定
	if(animations.count(animationName) == 0)
	{
		startTime.SetTime(0, 0, 0, 0, 0, FbxTime::EMode::eFrames60);
		endTime.SetTime(0, 0, 0, 0, 0, FbxTime::EMode::eFrames60);
		frameTime.SetTime(0, 0, 0, 0, 0, FbxTime::EMode::eFrames60);
		return;
	}
	nowAnimationName = animationName;
	startTime = animations[animationName].startTime;
	endTime = animations[animationName].endTime;
	currentTime = startTime;

	if (startTime > endTime)
		frameTime.SetTime(0, 0, 0, -FrameTime, 0, FbxTime::EMode::eFrames60);
	else
		frameTime.SetTime(0, 0, 0, FrameTime, 0, FbxTime::EMode::eFrames60);

}

bool FBXModel::PlayAnimation(bool endless)
{
	if (!isPlay)
		return false;

	currentTime += frameTime;
	if ((currentTime >= endTime && frameTime > 0) || (currentTime <= endTime && frameTime < 0))
	{
		if (!endless)
		{
			currentTime = endTime;
			isPlay = false;
			return false;
		}
		currentTime = startTime;
	}
	//定数バッファのデータ転送
	ConstBufferDataSkin* constMapSkin = nullptr;
	HRESULT result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	assert(SUCCEEDED(result));
	for (int i = 0; i < bones.size(); i++)
	{
		//今の姿勢
		XMMATRIX matCurrentPose;
		//今の姿勢行列を取得
		FbxAMatrix fbxCurrentPose =
			bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);
		//XMMATRIXに変換
		FbxLoader::ConvertMatrixFromFbx(&matCurrentPose, fbxCurrentPose);
		//合成してスキニングして行列に
		constMapSkin->bones[i] = bones[i].invInitialPose * matCurrentPose;
	}
	constBuffSkin->Unmap(0, nullptr);


	return true;
}

bool FBXModel::PlayAnimation(const std::string& animationName, bool endless, const int frameTime,bool blend)
{
	if (nowAnimationName == animationName)
	{
		return PlayAnimation(endless);
	}
	if (!blend)
	{
		nowAnimationName = animationName;
		lerpAnimeName = animationName;
		SetAnimation(animationName, frameTime);
		lerpTime = 0.0f;
		return PlayAnimation(endless);
	}

	const auto nextAnimationTime = animations[animationName].startTime;
	if(lerpAnimeName != animationName)
	{
		lerpAnimeName = animationName;
		motionBlendTime = ClucLerpTime(currentTime, nextAnimationTime);
		
	}
	lerpTime += 1.0f / motionBlendTime;
	//定数バッファのデータ転送
	ConstBufferDataSkin* constMapSkin = nullptr;
	HRESULT result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	assert(SUCCEEDED(result));
	for (int i = 0; i < bones.size(); i++)
	{

		XMMATRIX matCurrentPose, matStartPose, matEndPose;
		////今の姿勢行列を取得
		//FbxAMatrix fbxStartPose =
		//	bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);
		////今の姿勢行列を取得
		//FbxAMatrix fbxEndPose =
		//	bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(nextAnimationTime);

		//XMMATRIXに変換
		FbxLoader::ConvertMatrixFromFbx(&matStartPose, blendStartBorn[i]);
		FbxLoader::ConvertMatrixFromFbx(&matEndPose, blendEndBorn[i]);
		matCurrentPose = SlerpMatrix(matStartPose, matEndPose, lerpTime);
		//合成してスキニングして行列に
		constMapSkin->bones[i] = bones[i].invInitialPose * matCurrentPose;
	}
	constBuffSkin->Unmap(0, nullptr);

	if (lerpTime >= 1.0f)
	{
		nowAnimationName = animationName;
		SetAnimation(animationName, frameTime);
		lerpTime = 0.0f;
	}

	return true;
}

XMMATRIX FBXModel::SlerpMatrix(const Matrix4& start, const Matrix4& end, float slerpAmount)
{
	if (start == end)
		return  Matrix4::convertToXMMATRIX(start);
	Quaternion qStart, qEnd, qResult;
	Vector3 curTrans, nextTrans, lerpedTrans;
	Vector3 curScale, nextScale, lerpedScale;
	Matrix4 startRotation, endRotation;
	Matrix4 returnMatrix;

	//回転行列はQuaternion.SlerpではなくQuaternion.Lerp
	qStart = quaternion(start);
	qEnd = quaternion(end);
	qResult = lerp(qStart, qEnd, slerpAmount);

	//平行移動行列はVector3.Lerpを使っている
	curTrans.x = start.m[3][0];
	curTrans.y = start.m[3][1];
	curTrans.z = start.m[3][2];
	nextTrans.x = end.m[3][0];
	nextTrans.y = end.m[3][1];
	nextTrans.z = end.m[3][2];

	lerpedTrans = Vector3::Lerp(curTrans, nextTrans, slerpAmount);

	//拡大縮小行列にはVector3.Lerp
	startRotation = rotate(qStart);
	endRotation = rotate(qEnd);
	curScale.x = start.m[0][0] - startRotation.m[0][0];
	curScale.y = start.m[1][1] - startRotation.m[1][1];
	curScale.z = start.m[2][2] - startRotation.m[2][2];
	nextScale.x = end.m[0][0] - endRotation.m[0][0];
	nextScale.y = end.m[1][1] - endRotation.m[1][1];
	nextScale.z = end.m[2][2] - endRotation.m[2][2];

	lerpedScale = Vector3::Lerp(curScale, nextScale, slerpAmount);

	//srt行列を作成してMatrix.CreateScale(S*R*T)と同じことをしている（こっちのが断然軽い）
	returnMatrix = rotate(qResult);
	returnMatrix.m[3][0] = lerpedTrans.x;
	returnMatrix.m[3][1] = lerpedTrans.y;
	returnMatrix.m[3][2] = lerpedTrans.z;
	returnMatrix.m[0][0] += lerpedScale.x;
	returnMatrix.m[1][1] += lerpedScale.y;
	returnMatrix.m[2][2] += lerpedScale.z;
	return Matrix4::convertToXMMATRIX(returnMatrix);
}

float FBXModel::ClucLerpTime(const FbxTime& current, const FbxTime& next)
{
	float result = 0.0f;
	for (int i = 0; i < bones.size(); i++)
	{
		XMMATRIX matCurrentPose, matStartPose, matEndPose;
		//今の姿勢行列を取得
		blendStartBorn[i] =
			bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(current);
		//今の姿勢行列を取得
		blendEndBorn[i] =
			bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(next);
		//XMMATRIXに変換
		FbxLoader::ConvertMatrixFromFbx(&matStartPose, blendStartBorn[i]);
		FbxLoader::ConvertMatrixFromFbx(&matEndPose, blendEndBorn[i]);

		matCurrentPose = matStartPose - matEndPose;

		Vector3 x = matCurrentPose.r[0];
		Vector3 y = matCurrentPose.r[1];
		Vector3 z = matCurrentPose.r[2];

		const float thisBoneSub = x.Length() + y.Length() + z.Length();

		if (result < thisBoneSub)
			result = thisBoneSub;
	}

	if (result == 0.0f)
		return 1.0f;
	
	return result * 2.5f;
}

