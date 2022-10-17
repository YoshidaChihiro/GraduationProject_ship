#pragma once
#include <DirectXMath.h>
#include <d3d12.h>

#include "Vector.h"
using namespace DirectX;

class Camera
{
public:
	Camera(const Vector2& windowSize = { 1920,1080 });
	Camera(const XMFLOAT3& eye, const XMFLOAT3& target, const XMFLOAT3& up,float arg_near = 0.1f,float arg_far = 1000.0f, const Vector2& windowSize = { 1920,1080 });
	~Camera();

	virtual void Update();
	void UpdateViewMatrix();
	inline const Vector3& GetEye() { return eye; }
	inline const Vector3& GetTarget() { return target; }
	inline const Vector3& GetUp() { return up; }

	inline const XMMATRIX& GetMatView() { return matView; }
	inline const XMMATRIX& GetMatProjection() { return matProjection; }
	inline const XMMATRIX& GetMatViewProjection() { 
		matViewProjection = matView * matProjection;
		return matViewProjection; }
	inline const XMMATRIX& GetMatBillboard() { return matBillboard; }
	inline const XMMATRIX& GetMatBillboardY() { return matBillboardY; }
	inline const float GetNear() { return nearDist; }
	inline const float GetFar() { return farDist; }
	inline const float GetPhi() { return phi; }
	inline const float GetTheta()
	{
		if (theta < 0)
			return 0;
		return theta;
	}

	void SetMatrixView(const XMFLOAT3& eye, const XMFLOAT3& target, const XMFLOAT3& up);
	void MoveVector(const XMVECTOR& moveeye = { }, const XMVECTOR& movetarget = { }, const XMVECTOR& movrup = { });
	void SetTarget(const XMFLOAT3& target) { this->target = target; viewDirty = true; }
	void SetEye(const XMFLOAT3& eye) { this->eye = eye; viewDirty = true; }
protected:
	XMMATRIX matView;//ビュー変換行列
	float angle = 0.0f;
	float nearDist = 0.1f;
	float farDist = 1000.0f;
	float phi;
	float theta;

	Vector3 eye;  //視点座標
	Vector3 target;  //視点座標
	Vector3 up;  //視点座標
	// ビュー行列ダーティフラグ
	bool viewDirty = false;
	//ビルボード行列
	XMMATRIX matBillboard = DirectX::XMMatrixIdentity();
	// Y軸回りビルボード行列
	XMMATRIX matBillboardY = DirectX::XMMatrixIdentity();
	// 射影行列
	XMMATRIX matProjection = DirectX::XMMatrixIdentity();
	// ビュー射影行列
	XMMATRIX matViewProjection = DirectX::XMMatrixIdentity();

};

