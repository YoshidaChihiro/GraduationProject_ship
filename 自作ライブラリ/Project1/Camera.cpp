#include "Camera.h"

Camera::Camera(const Vector2& windowSize)
{
	float aspectRatio = (float)windowSize.x / windowSize.y;

	eye = XMFLOAT3(0, 0, -10);
	target = XMFLOAT3(0, 0, 0);
	up = XMFLOAT3(0, 1, 0);
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),
		aspectRatio,
		nearDist, farDist
	);

	UpdateViewMatrix();
}

Camera::Camera(const XMFLOAT3& eye,const XMFLOAT3& target,const XMFLOAT3& up,float arg_near , float arg_far, const Vector2& windowSize ):eye(eye),target(target),up(up),nearDist(arg_near),farDist(arg_far)
{
	float aspectRatio = (float)windowSize.x / windowSize.y;
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),
		aspectRatio,
		nearDist, farDist
	);

	UpdateViewMatrix();
}


Camera::~Camera()
{
}

void Camera::Update()
{
	if (viewDirty) {
		// ビュー行列更新
		UpdateViewMatrix();
		viewDirty = false;
	}
}

void Camera::UpdateViewMatrix()
{
	// 視点座標
	XMVECTOR eyePosition = XMLoadFloat3(&eye);
	// 注視点座標
	XMVECTOR targetPosition = XMLoadFloat3(&target);
	// （仮の）上方向
	XMVECTOR upVector = XMLoadFloat3(&up);

	// カメラZ軸（視線方向）
	XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
	// 0ベクトルだと向きが定まらないので除外
	assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVector));
	// ベクトルを正規化
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);

	// カメラのX軸（右方向）
	XMVECTOR cameraAxisX;
	// X軸は上方向→Z軸の外積で求まる
	cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
	// ベクトルを正規化
	cameraAxisX = XMVector3Normalize(cameraAxisX);

	// カメラのY軸（上方向）
	XMVECTOR cameraAxisY;
	// Y軸はZ軸→X軸の外積で求まる
	cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);

	// ここまでで直交した3方向のベクトルが揃う
	//（ワールド座標系でのカメラの右方向、上方向、前方向）	

	// カメラ回転行列
	XMMATRIX matCameraRot;
	// カメラ座標系→ワールド座標系の変換行列
	matCameraRot.r[0] = cameraAxisX;
	matCameraRot.r[1] = cameraAxisY;
	matCameraRot.r[2] = cameraAxisZ;
	matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
	// 転置により逆行列（逆回転）を計算
	matView = XMMatrixTranspose(matCameraRot);

	// 視点座標に-1を掛けた座標
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
	// カメラの位置からワールド原点へのベクトル（カメラ座標系）
	XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);	// X成分
	XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);	// Y成分
	XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);	// Z成分
	// 一つのベクトルにまとめる
	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
	// ビュー行列に平行移動成分を設定
	matView.r[3] = translation;

#pragma region 全方向ビルボード行列の計算
	// ビルボード行列
	matBillboard.r[0] = cameraAxisX;
	matBillboard.r[1] = cameraAxisY;
	matBillboard.r[2] = cameraAxisZ;
	matBillboard.r[3] = XMVectorSet(0, 0, 0, 1);
#pragma region

#pragma region Y軸回りビルボード行列の計算
	// カメラX軸、Y軸、Z軸
	XMVECTOR ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

	// X軸は共通
	ybillCameraAxisX = cameraAxisX;
	// Y軸はワールド座標系のY軸
	ybillCameraAxisY = XMVector3Normalize(upVector);
	// Z軸はX軸→Y軸の外積で求まる
	ybillCameraAxisZ = XMVector3Cross(ybillCameraAxisX, ybillCameraAxisY);

	// Y軸回りビルボード行列
	matBillboardY.r[0] = ybillCameraAxisX;
	matBillboardY.r[1] = ybillCameraAxisY;
	matBillboardY.r[2] = ybillCameraAxisZ;
	matBillboardY.r[3] = XMVectorSet(0, 0, 0, 1);
#pragma endregion

}

void Camera::SetMatrixView(const XMFLOAT3 & eye, const XMFLOAT3 & target, const XMFLOAT3 & up)
{
	this->eye = eye;
	this->target = target;
	this->up = up;
	viewDirty = true;
}

void Camera::MoveVector(const XMVECTOR & moveeye, const XMVECTOR & movetarget, const XMVECTOR & moveup)
{
	eye.x += moveeye.m128_f32[0];
	eye.y += moveeye.m128_f32[1];
	eye.z += moveeye.m128_f32[2];

	target.x += movetarget.m128_f32[0];
	target.y += movetarget.m128_f32[1];
	target.z += movetarget.m128_f32[2];

	up.x += moveup.m128_f32[0];
	up.y += moveup.m128_f32[1];
	up.z += moveup.m128_f32[2];
	viewDirty = true;
}

