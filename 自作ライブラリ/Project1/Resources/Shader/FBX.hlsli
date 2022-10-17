#include "LightStruct.hlsli"
cbuffer cbuff0 : register(b0)
{
	//matrix mat; // ３Ｄ変換行列
	matrix viewproj;//ビュープロジェクション行列
	matrix world;//ワールド行列
	float3 cameraPos;//カメラ座標(ワールド座標)
	float4 color;
};

cbuffer cbuff1 : register(b1)
{
    matrix lightCameraVP; //ビュープロジェクション行列
    float3 lightCameraPos; //カメラ座標(ワールド座標)
}
cbuffer cbuff2 : register(b2)
{
    float3 ambientColor;
    DirLight dirLights[DIRLIGHT_NUM];
    PointLight pointLights[POINTLIGHT_NUM];
    SpotLight spotLights[SPOTLIGHT_NUM];
    CircleShadow circleShadows[CIRCLESHADOW_NUM];
}

static const int MAX_BONES = 32;
cbuffer skinning:register(b3)
{
    matrix matSkinning[MAX_BONES];
}
// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos : SV_POSITION; // システム用頂点座標
    float4 shadowPos : TEXCOORD1; //影座標
    float4 worldpos : POS; // ワールド座標
	float3 normal:NORMAL;//法線
	float2 uv  :TEXCOORD; // uv値
};
struct VSInput
{
    float4 svpos : POSITION; // システム用頂点座標	
    float3 normal : NORMAL; //法線
    float2 uv : TEXCOORD; // uv値
    uint4 boneIndices : BONEINDICES;
    float4 boneWeights : BONEWEIGHTS;
};
struct PSOutPut
{
    float4 color : SV_TARGET0; //通常のレンダリング結果
    float4 normal : SV_TARGET1; //法線
    float4 depth : SV_TARGET2; //深度値	
};

