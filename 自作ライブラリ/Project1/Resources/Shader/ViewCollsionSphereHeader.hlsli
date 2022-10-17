struct VSOutput
{
    float4 center : CENTER;
    float radius : RADIUS;
};
struct GSOutput
{
    float4 pos : SV_POSITION;
};

cbuffer cbuff0 : register(b0)
{
    matrix mat; // ビュープロジェクション行列
};

