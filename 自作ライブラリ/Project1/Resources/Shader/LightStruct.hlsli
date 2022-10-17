// 平行光源の数
static const int DIRLIGHT_NUM = 3;
struct DirLight
{
    float3 lightv; // ライトへの方向の単位ベクトル
    float3 lightcolor; // ライトの色(RGB)
};

//点光源
static const int POINTLIGHT_NUM = 9;
struct PointLight
{
    float3 lightpos; //ライト座標
    float3 lightcolor; //ライトの色
    float3 lightatten; //ライトの距離減衰係数
    uint active;
};

//スポットライト
static const int SPOTLIGHT_NUM = 3;
struct SpotLight
{
    float3 lightv; // ライトへの方向の単位ベクトル
    float3 lightpos; //ライト座標
    float3 lightcolor; //ライトの色
    float3 lightatten; //ライトの距離減衰係数
    float2 lightfactoranglecos; //ライト減衰角度のコサイン
    uint active;
};

//丸影の数
static const int CIRCLESHADOW_NUM = 1;

struct CircleShadow
{
    float3 dir; // 投影方向の逆ベクトル(単位ベクトル)
    float3 casterPos; //キャスター座標
    float distanceCasterLight; //キャスターとライトの距離
    float3 atten; //距離減衰係数
    float2 factorAngleCos; //減衰角度のコサイン
    uint active;

};
