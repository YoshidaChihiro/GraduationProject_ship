#include "BoxCollider.h"

void BoxCollider::Update()
{
	//付随しているオブジェクトの姿勢行列を取得
	const XMMATRIX& matWorld = object->GetMatWorld();
	//オブジェクトの中心の取得
	objCenter = matWorld.r[3];
	//オブジェクトの回転度合い
	auto matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	//コライダーの回転度合い
	auto colMatRot = XMMatrixIdentity();
	colMatRot *= XMMatrixRotationZ(XMConvertToRadians(colliderRot.z));
	colMatRot *= XMMatrixRotationX(XMConvertToRadians(colliderRot.x));
	colMatRot *= XMMatrixRotationY(XMConvertToRadians(colliderRot.y));

	Box::scale = scale;
	//コライダーの中心位置の計算
	ClucCenter(matRot);
	//コライダーのMAXとMINの計算
	ClucMinMaxPoint(colMatRot,matRot);
	//面方向の計算
	ClucDirect(colMatRot,matRot);
}



void BoxCollider::ClucCenter(const XMMATRIX& matRot)
{
	const XMMATRIX& matWorld = object->GetMatWorld();

	auto rotOffset = offset;
	//オブジェクトの回転度合いに合わせて回転
	rotOffset = XMVector3TransformNormal(rotOffset, matRot);

	Box::center = matWorld.r[3] + rotOffset;
}

void BoxCollider::ClucMinMaxPoint(const XMMATRIX&colMatRot,const XMMATRIX& matRot)
{
	//スケール情報から回転、平行移動前のローカル座標系でのMAXとMINを算出
	XMVECTOR minOffset = -XMVECTOR{ scale.x, scale.y, scale.z, 0 };
	XMVECTOR maxOffset = XMVECTOR{ scale.x, scale.y, scale.z, 0 };

	Box::rotation = object->GetRotation();
	//コライダーの回転度合いに合わせて回転
	minOffset = XMVector3TransformNormal(minOffset, colMatRot);
	maxOffset = XMVector3TransformNormal(maxOffset, colMatRot);

	//オブジェクトの中心位置からのオフセットを加算
	minOffset += offset;
	maxOffset += offset;
	
	//オブジェクトの回転度合いに合わせて回転
	minOffset = XMVector3TransformNormal(minOffset, matRot);
	maxOffset = XMVector3TransformNormal(maxOffset, matRot);

	//ワールド座標系に変換
	Box::minPosition = objCenter + minOffset;
	Box::maxPosition = objCenter + maxOffset;

}

void BoxCollider::ClucDirect(const XMMATRIX& colMatRot, const XMMATRIX& matRot)
{
	//回転前の各軸方向
	XMVECTOR x = { 1,0,0,0 };
	XMVECTOR y = { 0,1,0,0 };
	XMVECTOR z = { 0,0,1,0 };

	//コライダーの回転度合いに合わせて回転
	x = XMVector3TransformNormal(x, colMatRot);
	y = XMVector3TransformNormal(y, colMatRot);
	z = XMVector3TransformNormal(z, colMatRot);
	//オブジェクトの回転度合いに合わせて回転
	x = XMVector3TransformNormal(x, matRot);
	y = XMVector3TransformNormal(y, matRot);
	z = XMVector3TransformNormal(z, matRot);
	//正規化して格納
	normaDirect[0] = Vector3(x).Normalize().ConvertXMVECTOR();
	normaDirect[1] = Vector3(y).Normalize().ConvertXMVECTOR();
	normaDirect[2] = Vector3(z).Normalize().ConvertXMVECTOR();
}
