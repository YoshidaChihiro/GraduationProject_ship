#include"SpriteShaderHeader.hlsli"

Texture2D<float4> tex0 : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー


float4 PSmain(VSOutput input) : SV_TARGET
{
    const float pi = 3.14159265358979323846;
    const float epsilon = 1e-6;

    const float fringeExp = 2.3;
    const float fringeScale = 0.02;
    const float distortionExp = 2.0;
    const float distortionScale = 0.65;

    float2 baseUV = input.uv;
    float2 fromCentre = baseUV - float2(0.5, 0.5);

	fromCentre.y *= 1080.0f / 1920.0f;
    float radius = length(fromCentre);
    fromCentre = radius > epsilon
        ? (fromCentre * (1.0 / radius))
        : float2(0, 0);
	
    float strength = 0.8f;
    
    float fringing = fringeScale * pow(radius, fringeExp) * strength;
    float distortion = distortionScale * pow(radius, distortionExp) * strength;
    
    float2 distortUV = baseUV - fromCentre * distortion;
		
    float4 returnColor= tex0.Sample(smp, distortUV + fringing) * color;

    return returnColor;


}
