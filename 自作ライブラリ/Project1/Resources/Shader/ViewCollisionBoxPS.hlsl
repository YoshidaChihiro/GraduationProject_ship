#include "ViewCollisionBoxHeader.hlsli"

float4 PSmain(GSOutput input) : SV_TARGET
{
	return float4(input.color, 1.0f);
}