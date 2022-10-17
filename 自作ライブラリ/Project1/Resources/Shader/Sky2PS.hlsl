#include"Sky.hlsli"
Texture2D<float4> tex0 : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー
Texture2D<float4> tex1 : register(t1); //1番スロットに設定されたテクスチャ
SamplerState smp1 : register(s1); //1番スロットに設定されたサンプラー

int uGhosts = 5; // number of ghost samples
float uGhostDispersal = 0.6; // dispersion factor

float4 lensFlare(float2 coord)
{
    float2 texcoord = -coord + float2(1, 1);
 
   // ghost vector to image centre:
    float2 ghostVec = (float2(0.5, 0.5) - texcoord) * uGhostDispersal;
   
   // sample ghosts:  
    float4 result = float4(0, 0, 0, 0);
    for (int i = 0; i < uGhosts; ++i)
    {
        float2 offset = frac(texcoord + ghostVec * float(i));
  
        float weight = length(float2(0.5f, 0.5f) - offset) / length(float2(0.5, 0.5f));
        weight = pow(1.0 - weight, 10.0);
          
        result += tex1.Sample(smp1, offset) * weight;
    }
    
    return result;
}

float4 PSmain(VSOutput input) : SV_TARGET
{
    float2 uv = input.uv;
    float4 returnColor = lensFlare(uv) * 4.0 + tex0.Sample(smp, uv);

    return returnColor;
}