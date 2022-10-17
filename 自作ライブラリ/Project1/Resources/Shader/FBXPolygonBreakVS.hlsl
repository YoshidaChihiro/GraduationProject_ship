
#include"FBXPolygonBreak.hlsli"

//VSOutput VSmain(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
//{

//	//法線にワールド行列に四つスケーリング・回転を適用
//    //float4 wnormal = normalize(mul(world, float4(normal, 0)));
//    float4 wpos = mul(world, pos);

//	//ピクセルシェーダ―に渡す値	
//    VSOutput output;
//	//output.svpos = mul(mat, pos);
    
//    output.svpos = pos;
//    output.worldpos = wpos;
//    output.normal = normal;
//    output.uv = uv;

//    return output;
//}
struct SkinOutPut
{
    float4 pos;
    float3 normal;
};
//スキニング計算
SkinOutPut ComputeSkin(VSInput input)
{
//ゼロクリア
    SkinOutPut output = (SkinOutPut) 0;

    uint iBone; //計算するボーン番号
    float weight; //ボーンウェイト(重み)
    matrix m; //スキニングウェイト

    iBone = input.boneIndices.x;
    weight = input.boneWeights.x;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.svpos);
    output.normal += weight * mul((float3x3) m, input.normal);

    iBone = input.boneIndices.y;
    weight = input.boneWeights.y;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.svpos);
    output.normal += weight * mul((float3x3) m, input.normal);

    iBone = input.boneIndices.z;
    weight = input.boneWeights.z;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.svpos);
    output.normal += weight * mul((float3x3) m, input.normal);

    iBone = input.boneIndices.w;
    weight = input.boneWeights.w;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.svpos);
    output.normal += weight * mul((float3x3) m, input.normal);


    return output;
}

VSOutput VSmain(VSInput input)
{
    SkinOutPut skinned = ComputeSkin(input);
	
	//法線にワールド行列に四つスケーリング・回転を適用
    float4 wnormal = normalize(mul(world, float4(skinned.normal, 0)));

	//ピクセルシェーダ―に渡す値	
    VSOutput output;
	//output.svpos = mul(mat, pos);
    output.svpos =  skinned.pos;
		//法線にワールド行列に四つスケーリング・回転を適用
    //float4 wnormal = normalize(mul(world, float4(normal, 0)));
    float4 wpos = mul(world, skinned.pos);
    output.worldpos = wpos;
    output.normal = wnormal.xyz;
    output.uv = input.uv;

    return output;
}