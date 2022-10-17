#include "ViewCollisionBoxHeader.hlsli"

struct VSInput
{
    float4 center : CENTER;
    float3 scale : SCALE;
    float4 objCenter : OBJCENTER;
    float3 rotation : ROTATION;
    float3 colRotation : COLROTATION;
};

VSOutput VSmain(VSInput input)
{
    VSOutput output;
    output.center = input.center;
    output.scale = input.scale;
    output.objCenter = input.objCenter;
    output.rotation = input.rotation * 3.14159265f / 180;
    output.colRotation = input.colRotation * 3.14159265f / 180;
	return output;
}