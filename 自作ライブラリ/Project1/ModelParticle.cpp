#include "ModelParticle.h"
#include "OBJLoader.h"
#include<random>

PieceParticle::PieceParticle(const Vector3& arg_position, const Vector3& arg_scale, const Vector3& arg_rotation, const DirectX::XMFLOAT4& arg_color, Object* arg_getActorObj)
{
    position = arg_position;
    scale = arg_scale;
    rotation = arg_rotation;
    color = arg_color;
    getActorObj = arg_getActorObj;

    Create(OBJLoader::GetModel("fieldPiece"));
    Initialize();
}

PieceParticle::~PieceParticle()
{
}

void PieceParticle::Initialize()
{
    Object::Update();
    timer = 0;

    velocity = Vector3(0, 1, 0);

    const float ROT_RANGE = 20.0f;
    addRotation.x = GetRandom(-ROT_RANGE, ROT_RANGE);
    addRotation.y = GetRandom(-ROT_RANGE, ROT_RANGE);
    addRotation.z = GetRandom(-ROT_RANGE, ROT_RANGE);

    addScale.x = scale.x / 50;
    addScale.z = scale.z / 50;

    speed = 0.5f;
}

void PieceParticle::Update()
{
    Vector3 actorPos = getActorObj->GetPosition();

    timer += 1;
    if (timer <= MOVE_TIMER)
    {
        //ˆÚ“®
        position += velocity;

        //‰ñ“]
        rotation += addRotation;
    }
    else
    {
        speed += 0.05f;

        //ˆÚ“®
        float tempVelY = velocity.y;
        velocity = Vector3::Normalize(actorPos - position);
        position += velocity * speed;

        //‰ñ“]
        rotation += addRotation;

        //Šgk
        scale += -addScale;

        //Žæ“¾æ‚Ì‹ß‚­‚É—ˆ‚½‚çÁ‹Ž
        const int GET_RANGE = 3;
        if (position.x >= (actorPos.x - GET_RANGE) && position.x <= (actorPos.x + GET_RANGE))
        {
            if (position.y >= (actorPos.y - GET_RANGE) && position.y <= (actorPos.y + GET_RANGE))
            {
                if (position.z >= (actorPos.z - GET_RANGE) && position.z <= (actorPos.z + GET_RANGE))
                {
                    Dead();
                }
            }
        }
    }


    Object::Update();
}

void PieceParticle::Draw()
{
    Object::Draw();
}

float PieceParticle::GetRandom(float arg_min, float arg_max)
{
    std::random_device rnd;
    std::mt19937_64 mt64(rnd());
    std::uniform_real_distribution<float> genRandFloat(arg_min, arg_max);
    return genRandFloat(mt64);
}

DeadEffectParticle::DeadEffectParticle(const Vector3& arg_position, const DirectX::XMFLOAT4& arg_color)
{
    position = arg_position;
    position.y += 50;
    color = arg_color;
    Create(OBJLoader::GetModel("DeadEffect"));
    Initialize();

    scale = Vector3(10, 100, 10);
    timer = 0;
    addscale = 10;
}

DeadEffectParticle::~DeadEffectParticle()
{
}

void DeadEffectParticle::Initialize()
{
    Object::Update();
}

void DeadEffectParticle::Update()
{
    timer++;
    if (timer >= 30)
    {
        //Dead();
    }

    addscale += 5;
    scale.y += addscale;
    scale.x -= 0.25f;
    scale.z -= 0.25f;
    if (scale.x <= 0.0f)
    {
        scale.x = 0.0f;
        scale.z = 0.0f;
    }
    //color.w -= 0.05f;
    Object::Update();
}

void DeadEffectParticle::Draw()
{
    Object::Draw();
}