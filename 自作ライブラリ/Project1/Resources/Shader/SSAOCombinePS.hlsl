#include"SpriteShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //通常カラー
Texture2D<float> texSSAO : register(t1); //SSAO
//-----------バイラテラルアップサンプリング用---------------
Texture2D<float4> normTex : register(t2);     //通常サイズの法線テクスチャ
Texture2D<float> depthTex : register(t3);     //通常サイズの深度テクスチャ
Texture2D<float4> halfNormTex : register(t4); //1/4サイズの法線テクスチャ
Texture2D<float> halfDepthTex : register(t5); //1/4サイズの深度テクスチャ
//---------------------------------------------------------
SamplerState smp : register(s0);

float Gaussian(float sigma, float x)
{
    return exp(-(x * x) / (2.0 * sigma * sigma));
}

float JoinedBilateralGaussianBlur(float2 uv)
{
    float g_sigmaX = 3;
    float g_sigmaV = 0.03 * -0.3 + 0.001;
    float g_sigmaY = 3;
    float w, h, miplevels;
    texSSAO.GetDimensions(0, w, h, miplevels);

    float2 g_pixelSize = (1.0f / w, 1.0f / h);

    const float c_halfSamplesX = 4.;
    const float c_halfSamplesY = 4.;

    float total = 0.0;
    float ret = 0.0;

    float2 pivot = texSSAO.Sample(smp, uv);
    
    for (float iy = -c_halfSamplesY; iy <= c_halfSamplesY; iy++)
    {
        float fy = Gaussian(g_sigmaY, iy);
        float offsety = iy * g_pixelSize.y;

        for (float ix = -c_halfSamplesX; ix <= c_halfSamplesX; ix++)
        {
            float fx = Gaussian(g_sigmaX, ix);
            float offsetx = ix * g_pixelSize.x;
            
            float2 value = texSSAO.Sample(smp, uv + float2(offsetx, offsety));
                        
            float fv = Gaussian(g_sigmaV, abs(value.y - pivot.y));
            
            total += fx * fy * fv;
            ret += fx * fy * fv * value.r;
        }
    }
        
    return ret / total;
}
  
float4 BilateralUpsampling(float2 uv)
{
	//フルサイズテクスチャの大きさ取得
    float texW, texH, texMiplevels;
    tex.GetDimensions(0, texW, texH, texMiplevels);
	//　　　　　　　　0 1  0 1  0 1
	//現在のテクセル数□□|□□|□□  を取得
	//　　　　　　　　□□|□□|□□
	//　　　　　　　　2 3  2 3  2 3
    int nTexcel = (int) (uv.x * texW) % 2 + (((int) (uv.y * texH) % 2) * 2);

    float4 returnColor = { 1, 1, 1, 1 };

	//縮小テクスチャの大きさ取得
    float w, h, miplevels;
    halfNormTex.GetDimensions(0, w, h, miplevels);

	//縮小テクスチャのテクセル差分を計算
    float dx = 1.0f / w;
    float dy = 1.0f / h;

	//バイラテラルフィルタ相当の重みを設定
    float4 vBilinearWeights[4] =
    {
			  // 0   1    2    3
        float4(9.0f / 16.0f, 3.0f / 16.0f, 3.0f / 16.0f, 1.0f / 16.0f), //0
    	float4(3.0f / 16.0f, 9.0f / 16.0f, 1.0f / 16.0f, 3.0f / 16.0f), //1
    	float4(3.0f / 16.0f, 1.0f / 16.0f, 9.0f / 16.0f, 3.0f / 16.0f), //2
    	float4(1.0f / 16.0f, 3.0f / 16.0f, 3.0f / 16.0f, 9.0f / 16.0f), //3
    };

	//縮小バッファの4テクセル分オフセットを設定
    float2 uvOffset[4] =
    {
		{ dx * -0.5f, dy * -0.5f},
		{ dx *  0.5f, dy * -0.5f},
		{ dx * -0.5f, dy *  0.5f},
		{ dx *  0.5f, dy *  0.5f},
    };

	//法線に対しての重みを初期化
    float vNormalWeights[4] =
    {
       0,0,0,0
    };
	//法線テクスチャのフルサイズから値をサンプリング
    float4 vNormalHiRes = normTex.Sample(smp, uv);
	//法線縮小テクスチャの現在UV周囲4ピクセル分について
    for (int i = 0; i < 4;i++)
    {
    	//縮小テクスチャから値をサンプリング
        float4 vNormalCoarse = halfNormTex.Sample(smp, uv + uvOffset[i]);
    	//重みの計算
        vNormalWeights[i] = dot(vNormalCoarse, vNormalHiRes);
        vNormalWeights[i] = pow(vNormalWeights[i], 32);
    }

	//深度値に対しての重みを初期化
    float vDepthWeights[4] =
    {
        0,0,0,0
    };
	//深度テクスチャのフルサイズから値をサンプリング
    float fDepthHiRes = depthTex.Sample(smp, uv);
	//深度縮小テクスチャの現在UV周囲4ピクセル分について
    for (int i = 0; i < 4; i++)
    {
    	//縮小テクスチャから値をサンプリング
        float fDepthCoarse = halfDepthTex.Sample(smp, uv + uvOffset[i]);
    	//重みの計算
        float fDepthDiff = fDepthHiRes - fDepthCoarse;
        vDepthWeights[i] = 1.0f / (2.22045e-016 + abs(fDepthDiff));
    }

	//最終的な重みを初期化
    float fTotalWeight = 0;
    float vUpsampled = 0;
    for (int i = 0; i < 4; i++)
    {
    	//4テクセル分の法線、深度、フィルタの重みを乗算
        float fWeight = vNormalWeights[i] * vDepthWeights[i] * vBilinearWeights[nTexcel][i];
        fTotalWeight += fWeight;
        vUpsampled += texSSAO.Sample(smp, uv + uvOffset[i]) * fWeight;
    }
	//0～1の間に収めるために正規化
    returnColor = vUpsampled /= fTotalWeight;
    return returnColor;
}
float4 PSmain(VSOutput input) : SV_TARGET
{
	//縮小バッファそのまま
    //float ssao = texSSAO.Sample(smp, input.uv);
    //float4 returnColor = float4(ssao, ssao, ssao, 1.0f);
    //return returnColor;

	//バイラテラルフィルターのみ
    //float filtered = JoinedBilateralGaussianBlur(input.uv);
    //const float4 ssaoColor = float4(float3(filtered, filtered, filtered), 1.0);

	//バイラテラルアップサンプリング
    float4 col = tex.Sample(smp, input.uv);
    const float4 ssaoColor = BilateralUpsampling(input.uv);
    float4 returnColor = float4(col.rgb * ssaoColor.rgb, col.a);
    return returnColor;
}


