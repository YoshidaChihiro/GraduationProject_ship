#include "AreaEffect.hlsli"

Texture2D<float4> tex0 : register(t0); // 0番スロットに設定されたテクスチャ

SamplerState smp0 : register(s0); // 0番スロットに設定されたサンプラー

float4 PSmain(VSOutput input) : SV_TARGET
{
    float4 texcolor1 = tex0.Sample(smp0, input.uv /** float2(0.5f, 1.0f)*/ + offsetTime);
    return texcolor1 * color;
}
