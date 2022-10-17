
cbuffer cbuff0 : register(b0)
{
    matrix viewproj; // ビュープロジェクション行列
    matrix world; // ワールド行列
    float3 cameraPos; // カメラ座標（ワールド座標）
    float4 color;
    matrix lightViewProj;
};
cbuffer cbuff2 : register(b2)
{
    float2 offsetTime;//UVスクロール用
};


struct VSOutput
{
    float4 svpos : SV_POSITION; // システム用頂点座標
    float2 uv : TEXCOORD; // uv値
};
