static const float PI = 3.141592654f;
static float3 N;
	//アルベド
static float3 baseColor = float3(1,1,1);
	//金属度
static float metalness = 1.0f;
	//鏡面反射強度
static float specular = 1.0f;
	//粗さ
static float roughness = 0.4f;

//Schlickによる近似
//f0とf90の値を(1-cosine)の5乗でlerpする
//f0 : 光が垂直に入射したときの反射率
//f90 : 光が平行に入射したときの反射率
//cosine ; 2ベクトルのなす各のコサイン(内積値)
float SchlickFresnel(float f0, float f90, float cosine)
{
    float m = saturate(1 - cosine);
    float m2 = m * m; //高速化のためpowを使わない
    float m5 = m2 * m2 * m;
    return lerp(f0, f90, m5);
}


float3 SchlickFresnel3(float3 f0, float3 f90, float cosine)
{
    float m = saturate(1 - cosine);
    float m2 = m * m;
    float m5 = m2 * m2 * m;
    return lerp(f0, f90, m5);
}


//UE4のGGX分布
//alpha : roughnessの2乗
//NdotH : 法線とハーフベクトルの内積
float DistributionGGX(float alpha, float NdotH)
{
    const float alpha2 = alpha * alpha;
    float t = NdotH * NdotH * (alpha2 - 1.0f) + 1.0f;
    return alpha2 / (PI * t * t);
}

//ディズニーのフラネル計算
float3 DisneyFresnel(float LdotH)
{
	//F項(フラネル : Fresnel)
	//輝度
    float luminance = 0.3f * baseColor.r + 0.6f * baseColor.g + 0.1f * baseColor.b;
	//色合い
    float3 tintColor = baseColor / luminance;
	//非金属の鏡面反射色を計算
    float3 nonMetalColor = specular * 0.08f * tintColor;
	//metalnessによる色補間 金属の場合はベースカラー
    float3 specularColor = lerp(nonMetalColor, baseColor, metalness);
	//NdotHの割合でSchlickFresnel補間
    return SchlickFresnel3(specularColor, float3(1, 1, 1), LdotH);
}

//UE4のSmithモデル
float GeometricSmith(float cosine)
{
    float k = (roughness + 1.0f);
    k = k * k / 8.0f;

    return cosine / (cosine * (1.0f - k) + k);
}

//鏡面反射の計算
float3 CookTorranceSpecular(float NdotL, float NdotV, float NdotH, float LdotH)
{
	//D項(分布:Distribution)
    float Ds = DistributionGGX(roughness * roughness, NdotH);

	//F項(フレネル:Fresnel)
    float3 Fs = DisneyFresnel(LdotH);

	//G項(幾何減衰:Geometry attenuation)
    float Gs = GeometricSmith(NdotL) * GeometricSmith(NdotV);

	//m項(分母)
    float m = 4.0f * NdotL * NdotV;

	//合成して鏡面反射の色を得る
    return Ds * Fs * Gs / m;
}

//双方向反射分布関数
float3 BRDF(float3 L, float3 V)
{
	//法線とライト方向の内積
    float NdotL = dot(N, L);
	//法線とカメラ方向の内積
    float NdotV = dot(N, V);
	//どちらかが90度以上であれば真っ黒を返す
    if (NdotL < 0 || NdotV < 0)
    {
        return float3(0, 0, 0);
    }
    //ライト方向とカメラ方向の中間(ハーフベクトル)
    float3 H = normalize(L + V);
	//法線とハーフベクトルの内積
    float NdotH = dot(N, H);
	//ライトとハーフベクトルの内積
    float LdotH = dot(L, H);
	
	//拡散反射率
    float diffuseReflectance = 1.0f / PI;
    float energyBias = 0.5f * roughness;
	
	//入射角が90度の場合の拡散反射率
    float Fd90 = energyBias + 2 * LdotH * LdotH * roughness;
	//入っていく時の拡散反射率
    float FL = SchlickFresnel(1.0f, Fd90, NdotL);
	//出ていく時の拡散反射率
    float FV = SchlickFresnel(1.0f, Fd90, NdotV);
	//入って出ていくまでの拡散反射率
    float energyFactor = lerp(1.0f, 1.0f / 1.51f, roughness);
    float Fd = FL * FV * energyFactor;
	
	//拡散反射項
    float3 diffuseColor = diffuseReflectance * Fd * baseColor * (1 - metalness);
    //鏡面反射項
    float3 specularColor = CookTorranceSpecular(NdotL, NdotV, NdotH, LdotH);
	//拡散反射と鏡面反射の合計で色が決まる
    return diffuseColor + specularColor;
}
