#include"NoShade.hlsli"

Texture2D<float4> tex0 : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float4 PSmain(VSOutput input) : SV_TARGET
{
	//テクスチャマッピング
	float4 texcolor = tex0.Sample(smp, input.uv);
	
	if(texcolor.w == 0)
        clip(-1);
	
	//シェーディングによる色
	//float4 shadecolor;
	////光沢度
	//const float shininess = 4.0f;
	////鏡面反射光
	//float3 ambient = m_ambient;
	////拡散反射光
	//float3 diffuse =  m_diffuse;
	////全て加算する
	//shadecolor.rgb = (ambient + diffuse);
	//shadecolor.a = m_alpha;

	//シェーディングによる色で描画
	return /*shadecolor **/ texcolor * color;
}