cbuffer cbuff0 : register(b0)
{
    matrix viewproj; // ビュープロジェクション行列
    matrix world; // ワールド行列
    float3 cameraPos; // カメラ座標（ワールド座標）
};

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
    float4 svpos : SV_POSITION; // システム用頂点座標
};

struct VSGrassOutput
{
    float4 svpos : SV_POSITION; // システム用頂点座標
    float2 uv : TEXCOORD; // uv値
};


struct VSInput
{
    float4 svpos : POSITION; // システム用頂点座標
    float3 normal : NORMAL; //法線
    float2 uv : TEXCOORD; // uv値
    uint4 boneIndices : BONEINDICES;
    float4 boneWeights : BONEWEIGHTS;
};

struct GSOutput
{
    float4 svpos : SV_POSITION; // システム用頂点座標
};

struct HS_CONTROL_POINT_OUTPUT
{
    float4 svpos : SV_POSITION; // システム用頂点座標
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[3] : SV_TessFactor;
    float InsideTessFactor : SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3


struct DS_OUTPUT
{
    float4 svpos : SV_POSITION; // システム用頂点座標
};

