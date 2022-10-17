#include "Particle.hlsli"

// 四角形の頂点数
static const uint vnum = 4;

// センターからのオフセットテーブル
static const float4 offset_array[vnum] =
{
	float4(-0.5f,-0.5f, 0, 0),	// 左下
	float4(-0.5f,+0.5f, 0, 0),	// 左上
	float4(+0.5f,-0.5f, 0, 0),	// 右下
	float4(+0.5f,+0.5f, 0, 0)	// 右上
};

// UVテーブル（左上が0,0　右下が1,1）
static const float2 uv_array[vnum] =
{
	float2(0, 1),	// 左下
	float2(0, 0),	// 左上
	float2(1, 1),	// 右下
	float2(1, 0) 	// 右上
};
static const matrix MatInitialize = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
};
// 点の入力から、四角形を出力
[maxvertexcount(vnum)]
void GSmain(
	point VSOutput input[1] : SV_POSITION,
	inout TriangleStream< GSOutput > output
)
{
	GSOutput element;
	element.color = input[0].color;

	float4 offset = float4(0, 0, 0, 0);

    for (uint i = 0; i < vnum; i++)
    {
		// 中心からのオフセットをスケーリング
        offset = offset_array[i];
        offset.x *= input[0].scale.x;
        offset.y *= input[0].scale.y;
    	
		
        const matrix rotationX =
        {
            1, 0, 0, 0,
			0, cos(input[0].rotation.x), -sin(input[0].rotation.x), 0,
			0, sin(input[0].rotation.x), cos(input[0].rotation.x), 0,
			0, 0, 0, 1
        };
        const matrix rotationY =
        {
            cos(input[0].rotation.y), 0, sin(input[0].rotation.y), 0,
			0, 1, 0, 0,
			-sin(input[0].rotation.y), 0, cos(input[0].rotation.y), 0,
			0, 0, 0, 1
        };
        const matrix rotationZ =
        {
            cos(input[0].rotation.z), -sin(input[0].rotation.z), 0, 0,
			sin(input[0].rotation.z), cos(input[0].rotation.z), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
        };
        matrix rotation = MatInitialize;
        rotation = mul(rotationZ, rotation);
        rotation = mul(rotationX, rotation);
        rotation = mul(rotationY, rotation);

    	//input[0].billboardActive == 1の時はビルボード行列をかける。0の時は上で計算した回転行列をかける
        offset = mul(lerp(rotation, matBillboard, step(0.5, input[0].billboardActive)),offset);
		
		// オフセット分ずらす（ワールド座標）
        element.svpos = input[0].pos + offset;
		// ビュープロジェクション変換
        element.svpos = mul(mat, element.svpos);
        element.uv = uv_array[i];
        output.Append(element);
    }
}