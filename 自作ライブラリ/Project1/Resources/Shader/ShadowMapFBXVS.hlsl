#include"ShadowMap.hlsli"

static const int MAX_BONES = 32;
cbuffer skinning : register(b1)
{
    matrix matSkinning[MAX_BONES];
}


//スキニング計算
float4 ComputeSkin(VSInput input)
{
//ゼロクリア
    float4 output = float4(0,0,0,0);

    uint iBone; //計算するボーン番号
    float weight; //ボーンウェイト(重み)
    matrix m; //スキニングウェイト

    iBone = input.boneIndices.x;
    weight = input.boneWeights.x;
    m = matSkinning[iBone];
    output += weight * mul(m, input.svpos);

    iBone = input.boneIndices.y;
    weight = input.boneWeights.y;
    m = matSkinning[iBone];
    output += weight * mul(m, input.svpos);

    iBone = input.boneIndices.z;
    weight = input.boneWeights.z;
    m = matSkinning[iBone];
    output += weight * mul(m, input.svpos);

    iBone = input.boneIndices.w;
    weight = input.boneWeights.w;
    m = matSkinning[iBone];
    output += weight * mul(m, input.svpos);


    return output;
}

VSOutput VSmain(VSInput input)
{
    float4 skinnedPos = ComputeSkin(input);
	
	//ピクセルシェーダ―に渡す値	
    VSOutput output;
	//output.svpos = mul(mat, pos);
    output.svpos = mul(mul(viewproj, world), skinnedPos);

    return output;
}