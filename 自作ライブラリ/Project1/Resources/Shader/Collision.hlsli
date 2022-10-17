
struct CheckResult
{
    int result;
    float distance;
    float3 inter;
};

static const CheckResult Init =
{
    0, 0, float3(0, 0, 0)
};


struct Ray
{
	// 始点
    float3 start;
	// 方向
    float3 dir;
};
static const Ray RayInit =
{
    float3(0, 0, 0), float3(0, 1, 0)
};

struct Plane
{
	// 法線ベクトル
    float3 normal;
	// 原点からの距離
    float distance;
};
static const Plane PlaneInit =
{
    float3(0, 1, 0), 0
};

struct Triangle
{
	// 頂点座標3つ
    float3 p0;
    float3 p1;
    float3 p2;
	// 法線ベクトル
    float3 normal;
};
static const Triangle TriangleInit =
{
    float3(1, 0, 0), float3(0, 1, 0), float3(0, 0, 1), float3(0, 1, 0)
};

CheckResult CheckRay2Plane(const Ray ray, const Plane plane)
{
    CheckResult result = Init;
	
    float d1 = dot(plane.normal, ray.dir);
    float d2 = dot(plane.normal, ray.start);
    int check = lerp(0, 1, step(0.00001f, d1 * d1));
    float t = lerp(100000, (plane.distance - d2) / d1, check);

    //if (t < 0)
    //{
    //    return result;
    //}

    //result.distance = t;
    
    //result.inter = ray.start + t * ray.dir;
    
    //result.result = 1;
    result.distance = t;
    
    result.inter = ray.start + t * ray.dir;
    
    result.result = lerp(1,0,step(1000,t));
    return result;
}

CheckResult CheckRay2Triangle(const Ray ray, const Triangle tri /*,const float cameraToTargetLength*/)
{
    CheckResult result = Init;
	// 三角形が乗っている平面を算出
    Plane plane;
    plane.normal = tri.normal;
    plane.distance = dot(tri.normal, tri.p0);
	// レイと平面が当たっていなければ、当たっていない
    const CheckResult ray2PlaneResult = CheckRay2Plane(ray, plane);
    //if (!ray2PlaneResult.result)
    //{
    //    return result;
    //}
	// レイと平面が当たっていたので、距離と交点が書き込まれた

	// レイと平面の交点が三角形の内側にあるか判定
    const float epsilon = 0.00001f; // 誤差吸収用の微小な値
    float3 interPlane = ray2PlaneResult.inter;
    float3 m = float3(0, 0, 0);
	// 辺p0_p1について
    float3 pt_p0 = tri.p0 - interPlane;
    float3 p0_p1 = tri.p1 - tri.p0;
    m = cross(pt_p0, p0_p1);
    float dot1 = dot(m, tri.normal);
	// 辺の外側
    int a = lerp(0, 1, step(-epsilon, dot1));
    //if (dot(m, tri.normal).x < -epsilon) {
    //    return result;
    //}

	// 辺p1_p2について
    float3 pt_p1 = tri.p1 - interPlane;
    float3 p1_p2 = tri.p2 - tri.p1;
    m = cross(pt_p1, p1_p2);
    float dot2 = dot(m, tri.normal);
	// 辺の外側
    int b = lerp(0, 1, step(-epsilon, dot2));
    //if (dot(m, tri.normal).x < -epsilon)
    //{
    //    return result;
    //}
	
	// 辺p2_p0について
    float3 pt_p2 = tri.p2 - interPlane;
    float3 p2_p0 = tri.p0 - tri.p2;
    m = cross(pt_p2, p2_p0);
    float dot3 = dot(m, tri.normal);
	// 辺の外側
    int c = lerp(0, 1, step(-epsilon, dot3));
    //if (dot(m, tri.normal).x < -epsilon)
    //{
    //    return result;
    //}
	
	//全てOKなら1そうでないなら0になる
    int d = a * b * c /** e*/ * ray2PlaneResult.result;

    result.inter = ray2PlaneResult.inter;
    result.distance = ray2PlaneResult.distance;
    result.result = d;
    return result;
}



//線分と直線の距離と最接近点を求める
CheckResult Calc2LineNearestDistAndPos(
    float3 pStart,
    float3 pEnd,
    float3 pp2,
    float3 pv2
)
{
    CheckResult result = Init;
	
    float3 p1 = pStart;
    float3 p2 = pp2;
    float3 startToEnd = pEnd - pStart;
	
    float3 v1 = normalize(startToEnd);
    float3 v2 = normalize(pv2);

    float D1 = dot(p2 - p1, v1);
    float D2 = dot(p2 - p1, v2);
    
    float3 crossVec = cross(v1, v2);
    float crossVecLength = length(crossVec);
    float Dv = crossVecLength * crossVecLength; 
    //Dv = Dv * Dv;
    float3 v = cross(p2 - p1, v1);

    float Dv2 = lerp(dot(v1, v2),0.99f, step(1, abs(dot(v1, v2))));
    //abs(dot(v1, v2)) >= 1 ? 0.99f : dot(v1, v2);
    float t1 = (D1 - D2 * Dv2) / (1.0f - Dv2 * Dv2);
    float t2 = (D2 - D1 * Dv2) / (Dv2 * Dv2 - 1.0f);

    float3
    Q1 = p1 + t1 * v1,
    Q2 = p2 + t2 * v2;

    float3 startToQ1 = Q1 - pStart;
    float3 startToQ1Normal = normalize(startToQ1);
    float startToQ1Length = length(startToQ1);
    float startToEndLength = length(startToEnd);

    float a = length(startToQ1Normal - v1); //aが0ならstart-endとstart-Q1は同じ向き2なら逆向き

	//逆向きならstart位置が最接近点,同じ向きならstart-endの距離とstart-Q1の距離を比較しstart-Q1の方が長かったらendが最接近点
    float3 inter = lerp(lerp(Q1, pEnd, step(startToEndLength, startToQ1Length)), pStart, step(1, a));

    //inter = lerp(Q1, pEnd, step(startToEndLength, startToQ1Length));//start-Q1
	
    result.distance = lerp(length(v), length(Q2 - Q1), step(0.000001f, Dv));
    result.inter = lerp(float3(1000, 1000, 1000), inter, step(0.000001f, Dv));
    result.result = lerp(0, 1, step(0.000001f, Dv));
    return result;
}

float3 CulcCheckPoint(Ray ray,Triangle tri)
{
    float3 result = float3(0, 0, 0);

	//const float cameraToTargetLength = length(TargetPos - cameraPos);
	
    CheckResult RayTriResult = CheckRay2Triangle(ray, tri);

    CheckResult RayP0P1Result = Calc2LineNearestDistAndPos(tri.p0, tri.p1, ray.start, ray.dir);
    CheckResult RayP1P2Result = Calc2LineNearestDistAndPos(tri.p1, tri.p2, ray.start, ray.dir);
    CheckResult RayP2P0Result = Calc2LineNearestDistAndPos(tri.p2, tri.p0, ray.start, ray.dir);


    //if(RayP0P1Result.result == 1)
    //{
    //    d = RayP0P1Result.inter;
    //    dis = RayP0P1Result.distance;
    //}
    float3 inter = lerp((tri.p0 + tri.p1 + tri.p2) / 3, RayP0P1Result.inter, step(0.5, RayP0P1Result.result));
    float dis = lerp(300000.0f, RayP0P1Result.distance, step(0.5, RayP0P1Result.result)); //0だったらあほみたいに大きな値を入れる

	//if(RayP1P2Result.result == 1)
	//{
	//	if(dis > RayP1P2Result.distance)
	//	{
 //           dis = RayP1P2Result.distance;
 //           d = RayP1P2Result.inter;
 //       }
	//}
    inter = lerp(inter, lerp(inter, RayP1P2Result.inter, step(RayP1P2Result.distance, dis)), step(0.5, RayP1P2Result.result));
    dis = lerp(dis, lerp(dis, RayP1P2Result.distance, step(RayP1P2Result.distance, dis)), step(0.5, RayP1P2Result.result));
    //if (RayP2P0Result.result == 1)
    //{
    //    if (dis > RayP2P0Result.distance)
    //    {
    //        d = RayP2P0Result.inter;
    //    }
    //}
    inter = lerp(inter, lerp(inter, RayP2P0Result.inter, step(RayP2P0Result.distance, dis)), step(0.5, RayP2P0Result.result));
	
    result = lerp(inter, RayTriResult.inter, step(0.5, RayTriResult.result));
    //result = inter;
	
    return result;
	
}

