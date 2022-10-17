#include "CollisionPrimitive.h"

#include "Matrix4.h"
#include "Quaternion.h"

using namespace DirectX;

void Triangle::ComputeNormal()
{
	XMVECTOR p0_p1 = p1 - p0;
	XMVECTOR p0_p2 = p2 - p0;

	// 外積により、2辺に垂直なベクトルを算出する
	normal = XMVector3Cross(p0_p1, p0_p2);
	normal = XMVector3Normalize(normal);
}

void Frustum::CalculateFrustumPlanes(const Camera& camera)
{
    // 0: Left, 1: Right, 2: Bottm, 3: Top
    Camera tmpCamera = camera;
    float a, b, c,d;
    XMMATRIX tmpMP = tmpCamera.GetMatProjection();
    XMMATRIX camMatWorld = XMMatrixInverse(nullptr, tmpCamera.GetMatView());
    const Vector3 cameraDirectionZ = Vector3(camMatWorld.r[2].m128_f32[0], camMatWorld.r[2].m128_f32[1], camMatWorld.r[2].m128_f32[2]);
    const Vector3 cameraDirectionY = Vector3(camMatWorld.r[1].m128_f32[0], camMatWorld.r[1].m128_f32[1], camMatWorld.r[1].m128_f32[2]);
    const Vector3 cameraDirectionX = Vector3(camMatWorld.r[0].m128_f32[0], camMatWorld.r[0].m128_f32[1], camMatWorld.r[0].m128_f32[2]);

    planes.plane[0].normal = Vector3(cameraDirectionX * XMMatrixRotationAxis(camMatWorld.r[1], -30*3.14159265f/180.0f * 1920/1080)).Normalize();
    planes.plane[0].pos = tmpCamera.GetEye();
    planes.plane[1].normal = Vector3((cameraDirectionX*-1) * XMMatrixRotationAxis(camMatWorld.r[1], 30 * 3.14159265f / 180.0f * 1920 / 1080)).Normalize();
    planes.plane[1].pos = tmpCamera.GetEye();
    planes.plane[2].normal = Vector3(cameraDirectionY * XMMatrixRotationAxis(camMatWorld.r[0], 30 * 3.14159265f / 180.0f )).Normalize();
    planes.plane[2].pos = tmpCamera.GetEye();
    planes.plane[3].normal = Vector3((cameraDirectionY*-1) * XMMatrixRotationAxis(camMatWorld.r[0], -30 * 3.14159265f / 180.0f)).Normalize();
    planes.plane[3].pos = tmpCamera.GetEye();	
	
    // near面
    {
        a = tmpMP.r[3].m128_f32[0] + tmpMP.r[2].m128_f32[0];
        b = tmpMP.r[3].m128_f32[1] + tmpMP.r[2].m128_f32[1];
        c = tmpMP.r[3].m128_f32[2] + tmpMP.r[2].m128_f32[2];

        Vector3 normal = Vector3(c, b, a)*-1;
        normal.Normalize();

        Vector3 pos = tmpCamera.GetEye() + (cameraDirectionZ * tmpCamera.GetNear());//カメラの向きベクトル
        planes.plane[4].normal = cameraDirectionZ;
        planes.plane[4].pos = pos;
    }

    // far面
    {
        a = tmpMP.r[3].m128_f32[0] - tmpMP.r[2].m128_f32[0];
        b = tmpMP.r[3].m128_f32[1] - tmpMP.r[2].m128_f32[1];
        c = tmpMP.r[3].m128_f32[2] - tmpMP.r[2].m128_f32[2];

        Vector3 normal = Vector3(c, b, a)*-1;
        normal.Normalize();

        Vector3 pos = tmpCamera.GetEye() + cameraDirectionZ * tmpCamera.GetNear() + cameraDirectionZ * tmpCamera.GetFar();//カメラの向きベクトル


        planes.plane[5].normal = cameraDirectionZ*-1;
        planes.plane[5].pos = pos;
    }

}

