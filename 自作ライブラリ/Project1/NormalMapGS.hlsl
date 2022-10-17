#include"NormalMap.hlsli"

[maxvertexcount(3)]
void GSmain(
	triangle VSOutput input[3] : SV_POSITION,
	inout TriangleStream<GSOutput> output
)
{

    for (uint i = 0; i < 3; i++)
    {
        GSOutput o;
        o.svpos = mul(mul(viewproj, world), vPos[i]);
        o.color = vColor[i];

        //UNITY_TRANSFER_FOG(o, o.vertex);

        output.Append(o);
    }

    output.RestartStrip();
}
