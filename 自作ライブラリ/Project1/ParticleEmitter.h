#pragma once
#include"ParticleManager.h"
#include"Camera.h"
#include "ObjectManager.h"

class Line;
class ParticleEmitter
{
private:
    static ParticleManager* particleManager;
    static ObjectManager* p_objectManager;

public:
    static void Initialize();

    static void SetObjectManager(ObjectManager* arg_objManager);

    //パネルカット時のエフェクト(arg_pos:生成場所, arg_vec:進行方向)
    static void CutEffect(const Vector3& arg_pos, const Vector3& arg_vec);

    //衝突時のエフェクト(arg_pos:生成場所)
    static void ShockEffect(const Vector3& arg_pos, const Vector3& arg_color);

    //パネル回収時エフェクト
    static void PieceGetEffect(const Vector3& arg_position, const Vector3& arg_scale, const Vector3& arg_rotation, const DirectX::XMFLOAT4& arg_color, Object* arg_getActorObj);

    //フィーバーエフェクト
    static void FeverEffect(const Vector3& arg_position);


    static void DeadEffect(const Vector3& arg_position, const DirectX::XMFLOAT4& arg_color);

private:
    static float GetRandom(float arg_min, float arg_max);
};