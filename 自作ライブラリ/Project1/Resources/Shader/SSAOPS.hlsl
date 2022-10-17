#include"SpriteShaderHeader.hlsli"
//SSAO処理のためだけのシェーダ
Texture2D<float4> normtex : register(t0); //1パス目の法線描画
Texture2D<float> depthtex : register(t1); //1パス目の深度テクスチャ

SamplerState smp : register(s0);


static const int trycnt = 12;

//元座標復元に必要
cbuffer sceneBuffer : register(b1)
{
    matrix view; //ビュー
    matrix proj; //プロジェクション
    matrix invproj; //逆プロジェクション
    float4 randNormal[trycnt];
};

//現在のUV値を元に乱数を返す
float random(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898f, 78.233f))) * 43758.5453f);
}
//SSAO(乗算用の明度のみ情報を返せればよい)
float PSmain(VSOutput input) : SV_Target
{
    float dp = depthtex.Sample(smp, input.uv); //現在のUVの深度

	//元の座標を復元する
    float4 respos = mul(invproj, float4(input.uv * float2(2, -2) + float2(-1, 1), dp, 1));
    respos.xyz = respos.xyz / respos.w;
    //float div = 0.0f;
    float ao = 0.0f;
    float3 norm = normalize((normtex.Sample(smp, input.uv).xyz * 2) - 1);
    const float radius = 0.05f;

	if(dp<1.0f)
    {
        for (int i = 0; i < trycnt; ++i)
        {

            float3 omega = randNormal[i].xyz;

            float dt = dot(norm, omega);
            float sgn = sign(dt);
            omega *= sgn;
            float4 rpos = mul(proj, mul(view, float4(respos.xyz + omega * radius, 1)));
            rpos.xyz /= rpos.w;
            ao += step(depthtex.Sample(smp, (rpos.xy + float2(1, -1)) * float2(0.5, -0.5)), rpos.z) * dt * sgn;

        }
        ao /= (float) trycnt;
    }
    return 1.0f - ao;
}