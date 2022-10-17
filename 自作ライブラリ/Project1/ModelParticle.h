#pragma once
#include "Vector.h"
#include "Object.h"

//フィールドピース用パーティクル
class PieceParticle : public Object
{
public:
    PieceParticle(const Vector3& arg_position, const Vector3& arg_scale, const Vector3& arg_rotation, const DirectX::XMFLOAT4& arg_color, Object* arg_getActorObj);
    ~PieceParticle();
    void Initialize() override;
    void Update() override;
    void Draw() override;

private:
    float GetRandom(float arg_min, float arg_max);

    const int MOVE_TIMER = 25;

    int timer;
    Object* getActorObj;
    Vector3 addRotation;
    Vector3 addScale;
    float speed;
};

//フィールドピース用パーティクル
class DeadEffectParticle : public Object
{
public:
    DeadEffectParticle(const Vector3& arg_position, const DirectX::XMFLOAT4& arg_color);
    ~DeadEffectParticle();
    void Initialize() override;
    void Update() override;
    void Draw() override;

private:
    int timer;
    float addscale;
};