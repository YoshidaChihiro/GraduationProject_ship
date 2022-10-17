#include"SpriteShaderHeader.hlsli"

Texture2D<float4> tex0 : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー
cbuffer cbuff1 : register(b1)
{
	//中心からの描画範囲
    float drawDistance;
};


float4 PSmain(VSOutput input) : SV_TARGET
{

    if (length(input.uv - float2(0.5f, 0.5f)) > drawDistance)
        discard;
    float4 returnColor = tex0.Sample(smp, input.uv) * color;

	//if(returnColor.w <= 0)
 //       discard;
    return returnColor;

}