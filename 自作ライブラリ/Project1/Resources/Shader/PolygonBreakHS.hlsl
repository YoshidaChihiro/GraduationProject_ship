
#include "Collision.hlsli"
#include "PolygonBreak.hlsli"

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VSOutput, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;

  //  Output.EdgeTessFactor[0] =
		//Output.EdgeTessFactor[1] =
		//Output.EdgeTessFactor[2] = 1;
		//Output.InsideTessFactor =  5;
    float3 retf;
    float ritf, uitf;
    uint tessellation = 1;
    uint onTesllation = 0;
    const float legth1 = length(ip[0].worldpos.xyz - ip[1].worldpos.xyz);
    onTesllation = lerp(0, lerp(1, 2, step(10, legth1)), step(6, legth1));
    const float legth2 = length(ip[1].worldpos.xyz - ip[2].worldpos.xyz);
    onTesllation = lerp(onTesllation, lerp(onTesllation, 2, step(10, legth2)), step(6, legth2));
    const float legth3 = length(ip[2].worldpos.xyz - ip[0].worldpos.xyz);
    onTesllation = lerp(onTesllation, lerp(onTesllation, 2, step(10, legth3)), step(6, legth3));
    Triangle tri = TriangleInit;
    tri.p0 = ip[0].worldpos.xyz;
    tri.p1 = ip[1].worldpos.xyz;
    tri.p2 = ip[2].worldpos.xyz;
    tri.normal = normalize(cross(ip[1].worldpos.xyz - ip[0].worldpos.xyz, ip[2].worldpos.xyz - ip[0].worldpos.xyz));
    Ray ray = RayInit;
    ray.start = cameraPos;
    ray.dir = normalize(TargetPos - cameraPos);
    ray.dir.y = lerp(-0.0001, ray.dir.y, step(ray.dir.y, -0.0001));
    const float3 checkPos = CulcCheckPoint(ray, tri);
    const float TargetPosToWposLength = length(TargetPos - checkPos);

    tessellation = lerp(1, 3 * onTesllation, step(TargetPosToWposLength, 15));
	
    ProcessTriTessFactorsAvg(tessellation, 1.0f, retf, ritf, uitf);
    //ProcessTriTessFactorsAvg(_Tessellation, 1, retf, ritf, uitf);
    Output.EdgeTessFactor[0] = retf.x;
    Output.EdgeTessFactor[1] = retf.y;
    Output.EdgeTessFactor[2] = retf.z;
    Output.InsideTessFactor = ritf;
    return Output;
}


[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT HSmain(
	InputPatch<VSOutput, NUM_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID)
{
    HS_CONTROL_POINT_OUTPUT Output;

    Output.svpos = ip[i].svpos;
    Output.uv = ip[i].uv;
    Output.normal = ip[i].normal;
    Output.worldpos = ip[i].worldpos;
	

    return Output;
}
