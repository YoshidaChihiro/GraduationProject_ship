#include"NoShade.hlsli"

Texture2D<float4> tex0 : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー


cbuffer cbuff2 : register(b2)
{
    float4 color1;
    float4 color2;
    float angle;
}


float4 PSmain(VSOutput input) : SV_TARGET
{
    float2 center = { 0.5f, 0.5f };
    float2 baseVector = { 0, -1.0f };

    float2 nowPoint = input.uv;
    float2 nowVector = nowPoint - center;
    float dis = length(nowVector);
    if (dis > 0.5f)
        clip(-1);
    nowVector = normalize(nowVector);
    float cro = baseVector.x * nowVector.y - baseVector.y * nowVector.x;
    float nowAngle = acos(dot(baseVector, nowVector));
    float4 texColor = color1;
    if (cro < 0 && angle > 3.14159265f)
    {
        if (3.14159265f - nowAngle < angle - 3.14159265f)
        {
            texColor = color2;
            return texColor * color;
        }
    }
        
    if (cro > 0 && nowAngle < angle)
    {
        texColor = color2;
    }
    return texColor * color;
}