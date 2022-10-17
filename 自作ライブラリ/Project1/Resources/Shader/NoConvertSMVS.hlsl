#include"ShadowMap.hlsli"

VSGrassOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD )
{
	//ピクセルシェーダ―に渡す値	
    VSGrassOutput output;
    output.svpos = pos;
    output.uv = uv;
    return output;
}