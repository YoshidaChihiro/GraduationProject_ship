
#include "ShadowMap.hlsli"

[domain("tri")]
DS_OUTPUT DSmain(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
    DS_OUTPUT Output;
    Output.svpos = float4(
		(patch[0].svpos.xyz * domain.x + patch[1].svpos.xyz * domain.y + patch[2].svpos.xyz * domain.z) /** (1 - _Destruction * rand.xxx)*/, 1);
	
    return Output;
}
