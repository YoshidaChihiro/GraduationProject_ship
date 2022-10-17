#include"ViewCollsionSphereHeader.hlsli"
struct VSInput
{
    float3 center : CENTER;
    float radius : RADIUS;
};
VSOutput VSmain( VSInput input) 
{
    VSOutput output;
    output.center = float4(input.center, 0);
    output.radius = input.radius;
	return output;
}