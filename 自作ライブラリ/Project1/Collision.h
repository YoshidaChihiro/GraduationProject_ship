#pragma once
#include"CollisionPrimitive.h"
#include"window.h"
class Collision
{
private:
	static Window* window;
	//一番長い軸の長さ返す
	static float CheckMostLongAxis(const XMVECTOR& vec);
public:
	static void SetWindow(Window* window) { Collision::window = window; }
	
	// 点と三角形の最近接点を求める
	static void ClosestPtPoint2Triangle(const DirectX::XMVECTOR& point, const Triangle& triangle, DirectX::XMVECTOR* closest);
	
	// 球と球の当たり判定
	static bool CheckSphere2Sphere(const Sphere& sphereA, const Sphere& sphereB, DirectX::XMVECTOR*inter = nullptr,DirectX::XMVECTOR * reject = nullptr);
	
	// 球と平面の当たり判定
	static bool CheckSphere2Plane(const Sphere& sphere, const Plane& plane, DirectX::XMVECTOR*inter = nullptr);
	
	// 球と法線付き三角形の当たり判定
	static bool CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle, DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR * reject = nullptr);
	
	//レイと平面の当たり判定
	static bool CheckRay2Plane(const Ray& lay, const Plane& plane, float* distance, DirectX::XMVECTOR* inter = nullptr);

	// レイと法線付き三角形の当たり判定
	static bool CheckRay2Triangle(const Ray& lay, const Triangle& triangle, float*distance = nullptr, DirectX::XMVECTOR* inter = nullptr);
	
	// レイと球の当たり判定
	static bool CheckRay2Sphere(const Ray& lay, const Sphere& sphere, float*distance = nullptr, DirectX::XMVECTOR* inter = nullptr);

	static bool CheckRay2OBB(const Ray& ray, const Box& obb, float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);

	
	//球と矩形の当たり判定
	static bool CheckSphereBox(const Sphere& sphere, const Box& box, DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);

	//矩形と矩形の当たり判定
	static bool CheckAABBAABB(const Box& box1, const Box& box2, DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);
	
	static bool CheckOBBOBB(const Box& box1, const Box& box2, DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);

	// 分離軸に投影された軸成分から投影線分長を算出
	static float LenSegOnSeparateAxis(const Vector3& Sep, const Vector3& e1,const Vector3& e2,const Vector3& e3 = Vector3{0,0,0});

	static bool CheckSphereCapsule(const Sphere& sphere, const Capsule& capsule);

	static bool CheckOBBPlane(const Box& obb,const Plane& plane, DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);

	static bool Detect(const Box& target, const Frustum& arg_frustum);
};

