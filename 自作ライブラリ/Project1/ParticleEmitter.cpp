#include "ParticleEmitter.h"
#include "Matrix4.h"
#include "ModelParticle.h"
#include<random>

ParticleManager* ParticleEmitter::particleManager = nullptr;
ObjectManager* ParticleEmitter::p_objectManager = nullptr;

void ParticleEmitter::Initialize()
{
    particleManager = ParticleManager::GetInstance();
    particleManager->Initialize();
}

void ParticleEmitter::SetObjectManager(ObjectManager* arg_objManager)
{
    p_objectManager = arg_objManager;
}

void ParticleEmitter::CutEffect(const Vector3& arg_pos, const Vector3& arg_vec)
{
    
    for (int i = 0; i < 75; i++)
    {
        //飛んでいく方向
        Vector2 vecRange = { -0.15f,0.15f };
        float vecRangeX = GetRandom(vecRange.x, vecRange.y);
        float vecRangeZ = GetRandom(vecRange.x, vecRange.y);

        Vector3 vel = { (arg_vec.x + vecRangeX), arg_vec.y, (arg_vec.z + vecRangeZ) };
        vel *= -1.0f;
        vel *= GetRandom(1.0f, 5.0f) * 0.5f;
        vel.y = GetRandom(0.5f, 1.0f);

        Vector3 acc = { 0.0f,-0.1f,0.0f };

        //発生範囲
        Vector3 vec = Vector3::Normalize(arg_vec);
        float range = GetRandom(-1.0f, 0.0f);
        Vector3 position = arg_pos + (vec * range);

        Particle* particle = new Particle();

        particle->parameter.accel = acc;
        particle->parameter.num_frame = 5;
        particle->parameter.s_alpha = 1.0f;
        particle->parameter.e_alpha = 1.0f;
        particle->parameter.scaleAce = { 0,0 };
        particle->parameter.s_scale = 0.5f;
        particle->parameter.e_scale = 0.5f;
        particle->parameter.s_color = { (255.0f / 255.0f),  (100.0f / 255.0f),  (1.0f / 255.0f) };
        particle->parameter.e_color = { (255.0f / 255.0f),  (1.0f / 255.0f),  (1.0f / 255.0f) };
        particle->parameter.s_rotation = { 0,0,0 };
        particle->parameter.e_rotation = { 0,0,0 };

        particle->vsParam.position = position;
        particle->vsParam.velocity = vel;
        particle->vsParam.rotation = Vector3(0, 0, 0);
        particle->vsParam.scale = 0.25f;
        particle->vsParam.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;
        particle->vsParam.billboardActive = 1;
        particle->vsParam.color = { (255.0f / 255.0f),  (100.0f / 255.0f),  (1.0f / 255.0f), 1.0f };
        particle->vsParam.frame = 0;
        particle->vsParam.isDead = false;

        particleManager->Add(particle, "particle");
    }
    for (int i = 0; i < 25; i++)
    {
        //飛んでいく方向
        Vector2 vecRange = { -0.15f,0.15f };
        float vecRangeX = GetRandom(vecRange.x, vecRange.y);
        float vecRangeZ = GetRandom(vecRange.x, vecRange.y);

        Vector3 vel = { (arg_vec.x + vecRangeX), arg_vec.y, (arg_vec.z + vecRangeZ) };
        vel *= -1.0f;
        vel *= GetRandom(1.0f, 5.0f) * 0.5f;
        vel.y = GetRandom(0.5f, 1.0f);

        Vector3 acc = { 0.0f,-0.1f,0.0f };

        //発生範囲
        Vector3 vec = Vector3::Normalize(arg_vec);
        float range = GetRandom(-1.0f, 0.0f);
        Vector3 position = arg_pos + (vec * range);

        Particle* particle = new Particle();

        particle->parameter.accel = acc;
        particle->parameter.num_frame = 3;
        particle->parameter.s_alpha = 1.0f;
        particle->parameter.e_alpha = 1.0f;
        particle->parameter.scaleAce = { 0,0 };
        particle->parameter.s_scale = 0.5f;
        particle->parameter.e_scale = 0.5f;
        particle->parameter.s_color = { (255.0f / 255.0f),  (255.0f / 255.0f),  (255.0f / 255.0f) };
        particle->parameter.e_color = { (255.0f / 255.0f),  (255.0f / 255.0f),  (255.0f / 255.0f) };
        particle->parameter.s_rotation = { 0,0,0 };
        particle->parameter.e_rotation = { 0,0,0 };

        particle->vsParam.position = position;
        particle->vsParam.velocity = vel;
        particle->vsParam.rotation = Vector3(0, 0, 0);
        particle->vsParam.scale = 0.25f;
        particle->vsParam.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;
        particle->vsParam.billboardActive = 1;
        particle->vsParam.color = { (255.0f / 255.0f),  (255.0f / 255.0f),  (255.0f / 255.0f), 1.0f };
        particle->vsParam.frame = 0;
        particle->vsParam.isDead = false;

        particleManager->Add(particle, "particle");
    }
    
}

void ParticleEmitter::ShockEffect(const Vector3& arg_pos, const Vector3& arg_color)
{
    
    //球状に拡散するオレンジパーティクル
    for (int i = 0; i < 40; i++)
    {
        //飛んでいく方向
        Vector3 vel = { GetRandom(-1.0f, 1.0f), GetRandom(-1.0f, 1.0f), GetRandom(-1.0f, 1.0f) };
        vel = Vector3::Normalize(vel);
        vel *= GetRandom(0.5f, 2.5f) * 0.325f;

        //発生範囲
        Vector3 position = arg_pos;

        float scale = 0.75f;

        Particle* particle = new Particle();

        particle->parameter.accel = Vector3(0, 0, 0);
        particle->parameter.num_frame = 10;
        particle->parameter.s_alpha = 1.0f;
        particle->parameter.e_alpha = 1.0f;
        particle->parameter.scaleAce = { 0,0 };
        particle->parameter.s_scale = scale;
        particle->parameter.e_scale = 0.25f;
        particle->parameter.s_color = { (255.0f / 255.0f),  (100.0f / 255.0f),  (1.0f / 255.0f) };
        particle->parameter.e_color = { (255.0f / 255.0f),  (1.0f / 255.0f),  (1.0f / 255.0f) };
        particle->parameter.s_rotation = { 0,0,0 };
        particle->parameter.e_rotation = { 0,0,0 };

        particle->vsParam.position = position;
        particle->vsParam.velocity = vel;
        particle->vsParam.rotation = Vector3(0, 0, 0);
        particle->vsParam.scale = scale;
        particle->vsParam.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;
        particle->vsParam.billboardActive = 1;
        particle->vsParam.color = { (255.0f / 255.0f),  (100.0f / 255.0f),  (1.0f / 255.0f), 1.0f };
        particle->vsParam.frame = 0;
        particle->vsParam.isDead = false;

        particleManager->Add(particle, "particle");
    }

    //円状に飛ぶパーティクル
    int amount = 72;
    float addangle = (float)(360.0f / (float)(amount));
    float angle = 0.0f;

    for (int i = 0; i < amount; i++)
    {
        Particle* particle = new Particle();

        float scale = 0.5f;

        float radius = angle * 3.14f / 180.0f;
        angle += addangle;
        Vector3 vec = Vector3::Normalize({ cosf(radius), 0.0f, sinf(radius) }) * 0.4f;

        particle->parameter.accel = (vec * -1.0f) * 0.05f;
        particle->parameter.num_frame = 20;
        particle->parameter.s_alpha = 0.3f;
        particle->parameter.e_alpha = 0.0f;
        particle->parameter.scaleAce = { 0,0 };
        particle->parameter.s_scale = 0.0f;
        particle->parameter.e_scale = scale;
        particle->parameter.s_color = { (arg_color.x / 255.0f),  (arg_color.y / 255.0f),  (arg_color.z / 255.0f) };
        particle->parameter.e_color = { (arg_color.x / 255.0f),  (arg_color.y / 255.0f),  (arg_color.z / 255.0f) };
        particle->parameter.s_rotation = { 0,0,0 };
        particle->parameter.e_rotation = { 0,0,0 };

        particle->vsParam.position = arg_pos;
        particle->vsParam.velocity = vec;

        particle->vsParam.rotation = Vector3(0, 0, 0);
        particle->vsParam.scale = scale;
        particle->vsParam.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;
        particle->vsParam.billboardActive = 1;
        particle->vsParam.color = { (arg_color.x / 255.0f),  (arg_color.y / 255.0f),  (arg_color.z / 255.0f), 1.0f };
        particle->vsParam.frame = 0;
        particle->vsParam.isDead = false;

        particleManager->Add(particle, "particle");
    }
}

void ParticleEmitter::PieceGetEffect(const Vector3& arg_position, const Vector3& arg_scale, const Vector3& arg_rotation, const DirectX::XMFLOAT4& arg_color, Object* arg_getActorObj)
{
    p_objectManager->Add(new PieceParticle(arg_position, arg_scale, arg_rotation, arg_color, arg_getActorObj));
}

void ParticleEmitter::FeverEffect(const Vector3& arg_position)
{

    const float RESPAWN_RANGE = 2.5f;
    float rangeX = GetRandom(-RESPAWN_RANGE, RESPAWN_RANGE);
    float rangeY = GetRandom(-RESPAWN_RANGE, RESPAWN_RANGE);
    float rangeZ = GetRandom(-RESPAWN_RANGE, RESPAWN_RANGE);

    Vector3 position = arg_position + Vector3(rangeX, rangeY, rangeZ);

    Particle* particle = new Particle();

    float scale = 1.0f;

    int random = GetRandom(0.0f, 10.0f);
    //大きいサイズ
    if (random % 2 == 0)
    {
        scale = 2.0f;
    }
    //小さいサイズ
    else
    {
        scale = 1.0f;
    }


    particle->parameter.accel = Vector3(0, 0, 0);
    particle->parameter.num_frame = 15;
    particle->parameter.s_alpha = 1.0f;
    particle->parameter.e_alpha = 0.0f;
    particle->parameter.scaleAce = { 0,0 };
    particle->parameter.s_scale = scale;
    particle->parameter.e_scale = scale;
    particle->parameter.s_color = { (255.0f / 255.0f),  (255.0f / 255.0f),  (255.0f / 255.0f) };
    particle->parameter.e_color = { (255.0f / 255.0f),  (255.0f / 255.0f),  (255.0f / 255.0f) };
    particle->parameter.s_rotation = { 0,0,0 };
    particle->parameter.e_rotation = { 0,0,0 };

    particle->vsParam.position = arg_position + Vector3(rangeX, rangeY, rangeZ);
    particle->vsParam.velocity = Vector3(0, 0.1f, 0);
    particle->vsParam.rotation = Vector3(0, 0, 0);
    particle->vsParam.scale = scale;
    particle->vsParam.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;
    particle->vsParam.billboardActive = 1;
    particle->vsParam.color = { (255.0f / 255.0f),  (255.0f / 255.0f),  (255.0f / 255.0f), 1.0f };
    particle->vsParam.frame = 0;
    particle->vsParam.isDead = false;


    particleManager->Add(particle, "FeverPlayerEffect");
}


void ParticleEmitter::DeadEffect(const Vector3& arg_position, const DirectX::XMFLOAT4& arg_color)
{
    particleManager->AddDeadEffect(new DeadEffectParticle(arg_position, arg_color));
}

float ParticleEmitter::GetRandom(float arg_min, float arg_max)
{
    std::random_device rnd;
    std::mt19937_64 mt64(rnd());
    std::uniform_real_distribution<float> genRandFloat(arg_min, arg_max);
    return genRandFloat(mt64);
}