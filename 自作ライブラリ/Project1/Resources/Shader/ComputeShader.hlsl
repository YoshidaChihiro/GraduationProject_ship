
struct InputData
{
		// 加速度
    float3 accel;
    float s_alpha;
	// スケール
    float2 scaleAce;
    float2 s_scale ;
    float2 e_scale ;
    float e_alpha ;
	// 終了フレーム
    int num_frame;

    float3 s_color;
    float pad;
    float3 s_rotation;
    float pad2;
	// 最終値
    float3 e_color;
    float pad3;
    float3 e_rotation;
    float pad4;
	//// 加速度
 //   float3 accel;
 //   // スケール
 //   float s_alpha;
	//// 初期値
 //   float3 s_color;
 //   float s_scale;
 //   float3 s_rotation;
 //   float s_alpha;
	//// 最終値
 //   float3 e_color;
 //   float e_scale;
 //   float3 e_rotation;
 //   float e_alpha;
	//// 終了フレーム
 //   int num_frame;
    //float3 pad;
};
struct OutputData
{
    float3 position;
	//死亡フラグ
    uint isDead;

    float3 rotation;
	//ビルボードするかどうか
    uint billboardActive;

    float4 color;
    float2 scale;
    float2 scaleVel;

    float3 velocity;
    uint frame;


 //   float3 position;
 //   float scale;
	
 //   float3 rotation;
	////ビルボードするかどうか
 //   uint billboardActive;
	
 //   float4 color;
	
 //   float3 velocity;
 //   uint frame;
	////死亡フラグ
 //   uint isDead;
 //   float scaleVel;

 //   float2 pad;
};

//入力データ
StructuredBuffer<InputData> g_inputData : register(t0);
StructuredBuffer<OutputData> g_inputData2 : register(t1);

//出力データ
RWStructuredBuffer<OutputData> g_outputData : register(u0);

//[RootSignature(RS)]
[numthreads(1, 1, 1)]
void CSmain(uint3 gID : SV_DispatchThreadID)
{
    InputData input = g_inputData[gID.x];
    OutputData input2 = g_inputData2[gID.x];
    float3 velocity = input2.velocity;
			// 進行度を0～1の範囲に換算
    const float f = (float) input.num_frame / input2.frame;

			// 速度に加速度を加算
    velocity += input.accel;
    //velocity = normalize(velocity);
			// 速度による移動
    input2.position += velocity;
    input2.velocity = velocity;

			// カラーの線形補間			//アルファ値の線形補間
    input2.color = float4(input.s_color + (input.e_color - input.s_color) / f, input.s_alpha + (input.e_alpha - input.s_alpha) / f);

			//スケール速度に加速度を加算
    input2.scaleVel += input.scaleAce;

			// スケールに速度を加算
    input2.scale += input2.scaleVel;

			// 回転の線形補間
    input2.rotation = float4(input.s_rotation + (input.e_rotation - input.s_rotation) / f, 0);
    input2.isDead = step(input.num_frame, input2.frame);
        	// 経過フレーム数をカウント
    input2.frame++;


	
	//出力データ
    OutputData output = input2;
    //output.frame = input2.frame;
    g_outputData[gID.x] = output;
}