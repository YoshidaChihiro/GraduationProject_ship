#include"NormalMap.hlsli"

Texture2D<float4> tex0 : register(t0); // 0番スロットに設定されたテクスチャ
Texture2D<float4> tex1 : register(t1); // 1番スロットに設定されたテクスチャ

SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

float4 PSmain(GSOutput input) : SV_TARGET
{
    float3 nMap = (tex1.Sample(smp, input.uv).xyz) * 2 - 1;
    nMap = normalize(nMap);
    float3 tangent = normalize(input.tangent);
    float3 binormal = normalize(input.binormal);
    float3 normal = normalize(input.normal);

    float3 lNormal = normalize(nMap.x * tangent + nMap.y * binormal + nMap.z * normal);

    float3 wNormal = mul( world, lNormal);

    float3 eyeDir = normalize(cameraPos - input.worldpos.xyz);
    float3 halfVec = normalize(dirLights[0].lightv + eyeDir);

    float intensity = saturate(dot(wNormal, halfVec));
    float phong = pow(intensity, 40);
    float4 texcolor = tex0.Sample(smp, input.uv);

    return phong * texcolor + texcolor;
}
	
	
