cbuffer cbuff0 : register(b0)
{
    matrix viewproj; // ビュープロジェクション行列
    matrix world; // ワールド行列
};


// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
    float4 svpos : SV_POSITION; // システム用頂点座標
    float3 normal : NORMAL;
    float2 uv : TEXCOORD; // uv値
};
struct PSOutPut
{
    float4 color : SV_TARGET0; //通常のレンダリング結果
    float4 normal : SV_TARGET1; //法線
};
