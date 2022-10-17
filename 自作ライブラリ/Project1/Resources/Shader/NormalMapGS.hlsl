#include"NormalMap.hlsli"

[maxvertexcount(3)]
void GSmain(
	triangle VSOutput input[3] : SV_POSITION,
	inout TriangleStream<GSOutput> output
)
{

   // 5次元→3次元頂点に
    float3 CP0[3] =
    {
        float3(input[0].svpos.x, input[0].uv.x, input[0].uv.y),
        float3(input[0].svpos.y, input[0].uv.x, input[0].uv.y),
        float3(input[0].svpos.z, input[0].uv.x, input[0].uv.y),
    };
    float3 CP1[3] =
    {
        float3(input[1].svpos.x, input[1].uv.x, input[1].uv.y),
        float3(input[1].svpos.y, input[1].uv.x, input[1].uv.y),
        float3(input[1].svpos.z, input[1].uv.x, input[1].uv.y),
    };
    float3 CP2[3] =
    {
        float3(input[2].svpos.x, input[2].uv.x, input[2].uv.y),
        float3(input[2].svpos.y, input[2].uv.x, input[2].uv.y),
        float3(input[2].svpos.z, input[2].uv.x, input[2].uv.y),
    };

   // 平面パラメータからUV軸座標算出
    float U[3], V[3];
    for (int i = 0; i < 3; ++i)
    {
        float3 V1 = CP1[i] - CP0[i];
        float3 V2 = CP2[i] - CP1[i];
        float3 ABC = cross(V1, V2);

        U[i] = -ABC.y / ABC.x;
        V[i] = -ABC.z / ABC.x;
    }
    float3 tangent = float3(U[0], U[1], U[2]);
    float3 binormal = float3(V[0], V[1], V[2]);
        	
        	
   // 正規化
    tangent = normalize(tangent);
    binormal = normalize(binormal);
    
	
    for (uint i = 0; i < 3; i++)
    {
        GSOutput o;
        o.svpos = mul(mul(viewproj, world), input[i].svpos);
        o.normal = input[i].normal;
        o.uv = input[i].uv;
        o.worldpos = input[i].worldpos;
        o.tangent = tangent;
        o.binormal = binormal;
    	
        output.Append(o);
    }

    output.RestartStrip();
}
