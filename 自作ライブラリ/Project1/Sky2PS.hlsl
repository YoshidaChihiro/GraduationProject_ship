#include"Sky.hlsli"


int uGhosts = 5; // number of ghost samples
float uGhostDispersal = 0.6; // dispersion factor

float4 lensFlare(float2 coord)
{
    float2 texcoord = -coord + float2(1,1);
    //float2 texelSize = float2(1. / iResolution.x, 1. / iResolution.y);
 
   // ghost vector to image centre:
    float2 ghostVec = (float2(0.5f,0.5f) - texcoord) * uGhostDispersal;
   
   // sample ghosts:  
    float4 result = float4(0,0,0,0);
    for (int i = 0; i < uGhosts; ++i)
    {
        float2 offset = frac(texcoord + ghostVec * float(i));
  
        float weight = length(float2(0.5f,0.5f) - offset) / length(float2(0.5f,0.5f));
        weight = pow(1.0 - weight, 10.0);
          
        result += texture(iChannel1, offset) * weight;
    }
    
    return result;
}

float4 main(VSOutput input) : SV_TARGET
{
    float2 uv = fragCoord.xy / iResolution.xy;
    //fragColor = texture(iChannel0, uv);
    float4 fragColor = lensFlare(uv) * 4.0 + texture(iChannel0, uv);

    return fragColor;
}