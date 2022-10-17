#include"ShadowMap.hlsli"

VSOutput VSmain(float4 pos : POSITION)
{
	//ピクセルシェーダ―に渡す値	
    VSOutput output;
	//output.svpos = mul(mat, pos);
    output.svpos = mul(mul(viewproj, world), pos);

    return output;
}


