#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>
#include"PipelineState.h"
#include"Camera.h"
#include "LightCamera.h"
#include"Model.h"
#include"LightGroup.h"
#include"Vector.h"
using namespace Microsoft::WRL;
using namespace DirectX;

enum BILLBOARD_TYPE
{
	NONE,//ビルボードなし
	NORMAL,//全方向
	Y_AXIS,//Y軸周り
};
class Object3D
{
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://静的メンバ関数
	static Object3D* Create(Model* model,Vector3& pos,Vector3& scale,Vector3& rot,XMFLOAT4& color);
	static void SetCamera(Camera* camera);
	static void SetLightCamera(LightCamera* camera) { lightCamera = camera; }
	static void SetLightGroup(LightGroup* lightGroup) { Object3D::lightGroup = lightGroup; }
	static void SetDrawShadow(const bool flag) { drawShadow = flag; }
	static void SetScreenDraw(const bool flag) { screenDraw = flag; }
	static bool GetScreenDraw() { return screenDraw; }

	inline static bool GetDrawShadow() { return drawShadow; }
	static LightGroup* GetLightGroup() { return Object3D::lightGroup; }

	inline static Camera* GetCamera() { return Object3D::camera; }
	inline static LightCamera* GetLightCamera() { return Object3D::lightCamera; }
	static void ClucLightViewProjection();
private://静的メンバ変数
	static Camera* camera;
	static LightCamera* lightCamera;
	//ライトカメラを使って描画を行うか
	static bool drawShadow;
	static bool screenDraw;
	static LightGroup* lightGroup;
	static XMMATRIX  lightViewProjection;
public:
//構造体
	struct ConstBufferData
	{
		XMMATRIX viewprojection;
		XMMATRIX world;
		XMFLOAT3 cameraPos;
		float pad;//パディング
		XMFLOAT4 color;
		XMMATRIX lightViewProjection;

	};
	struct LightCameraCBData
	{
		XMMATRIX viewprojection;
		XMMATRIX world;
	};
	struct ConstLightCameraBuff
	{
		XMMATRIX viewProjection;
		XMFLOAT3 cameraPos;
	};

//メンバ関数
	Object3D(Vector3& pos, Vector3& scale, Vector3& rot, XMFLOAT4& color);
	bool Initialize();
	void Update(BILLBOARD_TYPE billboardType = NONE);
	void Draw(const bool fbx = false , const bool shade = true, BLENDTYPE type = ALPHA, const bool customPipeline = false, const int lightRootParameterIndex = 3);
	void SetModel(Model* model) { this->model = model; }
	void WorldUpdate(const Vector3& rot, const BILLBOARD_TYPE billboardType);

	const XMMATRIX& GetMatWorld() { return matWorld; }

	const XMMATRIX& GetRotateMat();
	//モデルを取得
	inline Model* GetModel() { return model; }
	//親オブジェクトをセット
	void SetParent(Object3D* parent) { this->parent = parent; }
private://メンバ変数
	ComPtr<ID3D12Resource> constBuff; // 定数バッファ
	ComPtr<ID3D12Resource> constBuff2; // 定数バッファ
	ComPtr<ID3D12Resource>lCameraConstBuff; // 定数バッファ
	ComPtr<ID3D12Resource>constCameraBuff;
	// ローカルワールド変換行列
	XMMATRIX matWorld;
	//ワールド行列に対する回転角度
	XMMATRIX appendRot = XMMatrixIdentity();
	// モデル
	Model* model = nullptr;
	//親オブジェクト
	Object3D* parent = nullptr;

	Vector3& position;
	Vector3& scale;
	Vector3& rotation;
	XMFLOAT4& color;

	
};




