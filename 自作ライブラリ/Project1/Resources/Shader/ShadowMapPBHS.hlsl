
#include "ShadowMap.hlsli"
cbuffer polygonBreak : register(b1)
{
    //float4 Status;
	
    float _Destruction; //分解度合い
    float _ScaleFactor; //スケールの変化量
    float _PositionFactor; //ポジションの変化量
    float _RotationFactor; //回転の変化量
    uint _Tessellation; //ポリゴン分割度

}

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
    ProcessTriTessFactorsAvg(1, 1.0f, retf, ritf, uitf);
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

    return Output;
}
