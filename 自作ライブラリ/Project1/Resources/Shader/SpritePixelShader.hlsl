#include"SpriteShaderHeader.hlsli"

Texture2D<float4> tex0:register(t0);//0番スロットに設定されたテクスチャ
SamplerState smp:register(s0);  //0番スロットに設定されたサンプラー

float4 PSmain(VSOutput input) :SV_TARGET
{
    float4 colorSRGB = tex0.Sample(smp, input.uv);
    float4 colorLINEAR = pow(colorSRGB, 2.2);
    float4 returnColor = colorLINEAR * color;
    returnColor = pow(returnColor, 1.0 / 2.2);
	//if(returnColor.w <= 0)
 //       discard;
    return returnColor;

}