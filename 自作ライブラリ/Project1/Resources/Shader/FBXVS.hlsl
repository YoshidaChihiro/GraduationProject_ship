#include"FBX.hlsli"

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
    output.svpos = mul(mul(viewproj, world), skinned.pos);
    output.worldpos = mul(world,skinned.pos);
    output.normal = wnormal.xyz;
    output.uv = input.uv;
    output.shadowPos = mul(mul(lightCameraVP, world), skinned.pos);

    return output;
}