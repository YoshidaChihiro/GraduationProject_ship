#pragma once

#include<DirectXMath.h>

class Object;
class BaseCollider;

//クエリーによる情報を得る為の構造体
struct QueryHit
{
	//衝突相手のオブジェクト
	Object* object = nullptr;
	//衝突相手のコライダー
	BaseCollider* collider = nullptr;
	//衝突点
	DirectX::XMVECTOR inter;
	//排斥ベクトル
	DirectX::XMVECTOR reject;
};

//クエリで交差を検出した時の動作を規定するクラス
class QueryCallback
{
public:
	QueryCallback() = default;
	virtual ~QueryCallback() = default;

	//交差時コールバック
	virtual bool OnQueryHit(const QueryHit& info) = 0;
};

