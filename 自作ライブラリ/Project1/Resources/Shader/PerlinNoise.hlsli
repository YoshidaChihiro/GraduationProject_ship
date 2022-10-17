float2 randomVec(float2 fact)
{
    float2 angle = float2(dot(fact, float2(127.1f, 311.7f)), dot(fact, float2(269.5f, 183.3f)));
    return frac(sin(angle) * 43758.5453123) * 2 - 1;
}
float PerlinNoize(float density, float2 uv)
{
    float2 uvFloor = floor(uv * density * float2(1920, 1080));
    float2 uvFrac = frac(uv * density * float2(1920, 1080));

    float2 v00 = randomVec(uvFloor + float2(0, 0));
    float2 v01 = randomVec(uvFloor + float2(0, 1));
    float2 v10 = randomVec(uvFloor + float2(1, 0));
    float2 v11 = randomVec(uvFloor + float2(1, 1));

    float c00 = dot(v00, uvFrac - float2(0, 0));
    float c01 = dot(v01, uvFrac - float2(0, 1));
    float c10 = dot(v10, uvFrac - float2(1, 0));
    float c11 = dot(v11, uvFrac - float2(1, 1));

    float2 u = uvFrac * uvFrac * (3 - 2 * uvFrac);

    float v0010 = lerp(c00, c10, u.x);
    float v0111 = lerp(c01, c11, u.x);

    return lerp(v0010, v0111, u.y) / 2 + 0.5;
}

float FractalSumNoise(float density, float2 uv)
{
    float fn;
    fn = PerlinNoize(density * 1, uv) * 1.0 / 2;
    fn += PerlinNoize(density * 2, uv) * 1.0 / 4;
    fn += PerlinNoize(density * 4, uv) * 1.0 / 8;
    fn += PerlinNoize(density * 8, uv) * 1.0 / 16;
    return fn;
}

float4 frag(float2 uv)
{
                // sample the texture
    float density = 0.01;
    float pn = FractalSumNoise(density, uv.xy);
				
    float4 col = float4(pn, pn, pn, 1);
    return col;
}
float random(in float2 _st)
{
    float2 rand = float2(FractalSumNoise(0.0, _st), FractalSumNoise(0.01, _st.yx));
    return frac(sin(dot(rand,
                         float2(12.9898, 78.233))) *
        43758.5453123);
}

