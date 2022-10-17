#pragma once
#include"Object3D.h"
#include"Vector.h"
#include"CollisionInfo.h"
#include<string>
class BaseCollider;
class Object
{
protected:
	//サブクラス
	struct HitBox
	{
	public:
		void SetPosition(const Vector3& pos) { position = pos; }
		void SetScale(const Vector3& scale) { this->scale = scale; }
		const Vector3& GetPosition() { return position; }
		const Vector3& GetScale() { return scale; }

	private:
		Vector3 position;
		Vector3 scale;
	};
	//当たり判定用Box
	HitBox hitBox;
	static int bbIndex;
public:	
	Object() = default;
	virtual ~Object();
	void Create(Model* model);
	virtual void Initialize();
	virtual void Update();
	virtual void DrawReady();
	virtual void Draw();
	void CustomDraw(const bool fbx = false, const bool shade = false, BLENDTYPE type = ALPHA,  const bool customPipeline = false, const int lightRootParameterIndex = 3);
	inline  const std::string& GetPipelineName()
	{
		return pipelineName;
	}

	void SetVelocity(const Vector3& arg_velocity) { velocity = arg_velocity; }
	inline Vector3 GetVelocity() { return velocity; }

	const XMMATRIX& GetMatWorld()
	{
		return object->GetMatWorld();
	}
	const XMMATRIX& GetRotateMat()
	{
		return object->GetRotateMat();
	}
	void SetCollider(BaseCollider* collider);
	virtual void OnCollision(const CollisionInfo& info){}

	void SetPosition(const Vector3& pos) { position = pos; }
	inline const Vector3& GetPosition() { return position; }
	void SetScale(const Vector3& scale) { this->scale = scale; }
	inline const Vector3& GetScale() { return scale; }
	void SetRotation(const Vector3& rot) { rotation = rot; }
	inline const Vector3& GetRotation() { return rotation; }
	void SetColor(const XMFLOAT4& color) { this->color = color; }
	inline const XMFLOAT4& GetColor() { return color; }
	inline const bool IsDead() { return dead; }
	inline const void Dead() { dead = true; }
	inline const char* GetName()const { return name; }
	void SetBillboardType(const BILLBOARD_TYPE billboardType) { this->billboardType = billboardType; }
	
protected:
	std::unique_ptr<Object3D> object;
	Vector3 position = { 0,0,0 };
	Vector3 scale = { 1,1,1 };
	Vector3 rotation = { 0,0,0 };
	Vector3 velocity = { 0,0,0 };
	Vector3 aceel = { 0,0,0 };
	XMFLOAT4 color = { 1,1,1,1 };
	std::string pipelineName = "BasicObj";
	//死亡フラグ
	bool dead = false;
	//クラス名(デバック用)
	const char* name = nullptr;
	//コライダー
	BaseCollider* collider = nullptr;
	//ビルボード
	BILLBOARD_TYPE billboardType = BILLBOARD_TYPE::NONE;
};

