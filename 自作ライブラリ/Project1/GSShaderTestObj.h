#pragma once
#include "Object.h"
class GSShaderTestObj :
    public Object
{
public:
	//GSShaderTestObj(const Vector3& pos);
	virtual void Draw() override;
	void DrawReady() override;
	static void CreateConstBuff();
	static void SendConstBuff();
	static void ChangeSendData();
protected:
	struct ConstBuffData
	{
		//XMFLOAT4 status;
		float _Destruction; //分解度合い
		float _ScaleFactor; //スケールの変化量
		float _PositionFactor;//ポジションの変化量
		float _RotationFactor;//回転の変化量
		int _Tessellation;//ポリゴン分割度
		int _OnEasing;//イージングするかどうか
		int _OnTargetBreak; //特定の位置のみ分解するかどうか
		int _OnCameraBreak;
		XMFLOAT3 BreakPoint; //分解中心点
		float BreakRadius; //分解半径
		XMFLOAT3 TargetPos;//カメラの注視点
		float CapsuleRadius;//カメラから注視点までのカプセル半径
		int _On4Vertex;
	};
	struct ConstLightCameraBuff
	{
		XMMATRIX viewProjection;
		XMFLOAT3 cameraPos;
	};

	static std::array<ComPtr<ID3D12Resource>,3> constBuff; // 定数バッファ
	static ConstBuffData sendData;
	static std::array<ComPtr<ID3D12Resource>, 3> constCameraBuff; // 定数バッファ
};

