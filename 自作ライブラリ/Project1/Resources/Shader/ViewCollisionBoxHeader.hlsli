struct VSOutput
{
    float4 center : CENTER;
    float3 scale : SCALE;
    float4 objCenter : OBJCENTER;
    float3 rotation : ROTATION;
    float3 colRotation : COLROTATION;
	
};
struct GSOutput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

cbuffer cbuff0 : register(b0)
{
    matrix mat; // ビュープロジェクション行列
};

