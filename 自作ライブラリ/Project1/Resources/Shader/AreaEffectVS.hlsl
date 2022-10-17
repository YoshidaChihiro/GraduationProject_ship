#include "AreaEffect.hlsli"

VSOutput VSmain(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	//ピクセルシェーダ―に渡す値	
    VSOutput output;
	//output.svpos = mul(mat, pos);
    output.svpos = mul(mul(viewproj, world), pos);
    output.uv = uv;

    return output;
}
