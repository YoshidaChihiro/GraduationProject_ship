#include "PerlinNoise.hlsli"
#include "ShadowMap.hlsli"

float random2(in float2 _st)
{
    return frac(sin(dot(_st.xy,
                         float2(12.9898, 78.233))) *
        43758.5453123);
}
cbuffer fbx : register(b1)
{
    float3 playerPos;
    float4 GrassBottomColor;
    float4 GrassTopColor;
    float2 AirDir;
    float AirPower;
    uint Time;
    float playerRadius;
    float GrassWidth;
    float GrassHeight;
	
}



[maxvertexcount(7)]
void GSmain(
	triangle VSGrassOutput input[3] : SV_POSITION,
	inout TriangleStream<GSOutput> output
)
{
	// ポリゴンの中心を計算。
    float4 center = (input[0].svpos + input[1].svpos + input[2].svpos) / 3;

    // ポリゴンの辺ベクトルを計算し、ポリゴンの法線を計算する。
    float3 vec1 = input[1].svpos - input[0].svpos;
    float3 vec2 = input[2].svpos - input[0].svpos;
    float3 up = normalize(cross(vec1, vec2));

    float4 dir = float4(random(input[2].svpos.xz * input[0].uv) * 2 - 1, 0, random(input[1].svpos.zx * input[2].uv.yx) * 2 - 1, 0.0f);
    dir = normalize(dir);
	
	//草の横並び頂点の幅(下からどのくらいずらすか)
    float WidthRate[3] = { 0.5, 0.4, 0.25 };
	//草の高さ(下からどのくらいずらすか)
    float HeightRate[3] = { 0.3, 0.4, 0.5 };

    //風の力
    float2 airForce;
    airForce = frag((input[0].uv + input[1].uv + input[2].uv) / 3 + AirDir * Time).xz;

    float4 vPos[7];
    float4 vColor[7];
    vPos[0] = center - dir * GrassWidth * WidthRate[0];
    vColor[0] = GrassBottomColor;

    vPos[1] = center - dir * GrassWidth * WidthRate[0];
    vColor[1] = GrassBottomColor;

    vPos[2] = center - dir * GrassWidth * WidthRate[1] + float4(up, 0) * GrassHeight * HeightRate[0] + float4(airForce.x, 0, airForce.y, 0) * AirPower * 0.3;
    vColor[2] = lerp(GrassBottomColor, GrassTopColor, 0.33333f);
    vPos[3] = center + dir * GrassWidth * WidthRate[1] + float4(up, 0) * GrassHeight * HeightRate[0] + float4(airForce.x, 0, airForce.y, 0) * AirPower * 0.3;
    vColor[3] = lerp(GrassBottomColor, GrassTopColor, 0.33333f);


    vPos[4] = vPos[3] - dir * GrassWidth * WidthRate[2] + float4(up, 0) * GrassHeight * HeightRate[1] + float4(airForce.x, 0, airForce.y, 0) * AirPower * 0.7;
    vColor[4] = lerp(GrassBottomColor, GrassTopColor, 0.6666f);
    vPos[5] = vPos[3] + dir * GrassWidth * WidthRate[2] + float4(up, 0) * GrassHeight * HeightRate[1] + float4(airForce.x, 0, airForce.y, 0) * AirPower * 0.7;
    vColor[5] = lerp(GrassBottomColor, GrassTopColor, 0.6666f);

    vPos[6] = vPos[5] + dir * GrassWidth * WidthRate[2] + float4(up, 0) * GrassHeight * HeightRate[2] + float4(airForce.x, 0, airForce.y, 0) * AirPower;
    vColor[6] = GrassTopColor;

    for (uint i = 0; i < 7; i++)
    {
        GSOutput o;
        o.svpos = mul(mul(viewproj, world), vPos[i]);
        output.Append(o);
    }

    output.RestartStrip();
}
