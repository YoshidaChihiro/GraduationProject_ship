#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include<wrl.h>
#include"DirectXLib.h"
#include"window.h"
#include"PipelineState.h"
#include"Vector.h"
using namespace Microsoft::WRL;

using namespace DirectX;
class Camera;
class Sprite3D
{
public:
	Sprite3D();
	void DrawSprite(const std::string& name, const Vector3& position, const float& rotation, const XMFLOAT2& scale, const XMFLOAT2& anchorPoint, bool billboard);
	void DrawSprite(const std::string& name, const Vector3& position, const float& rotation, const XMFLOAT2& scale, const XMFLOAT4& color, const XMFLOAT2& anchorPoint, bool billboard );
	void DrawSprite(const std::string& name, const Vector3& position, const float& rotation = 0.0f, const XMFLOAT2& scale = { 1.0f,1.0f }, const XMFLOAT4& color = { 1,1,1,1 }, const XMFLOAT2& anchorPoint = { 0.5f,0.5f }, const std::string& pipelineName = "NoShade", BLENDTYPE type = ALPHA,bool billboard = true);
	
	void DrawSpriteCutEffect(const std::string& name, const Vector3& position, const float& rotation, const Vector3& fieldRotation, const XMFLOAT2& scale, const XMFLOAT2& anchorPoint, bool billboard, const DirectX::XMFLOAT4& arg_color);

	
	//void NoPipelineDraw(const std::string& name, const XMFLOAT2& position, const float& rotation, const XMFLOAT2& scale, const XMFLOAT4& color, const XMFLOAT2& anchorPoint);
	void SpriteSetTextureRect(const std::string& name, const float& tex_x, const float& tex_y, const float& tex_width, const float& tex_height);
	static void SetCamera(Camera* camera)
	{
		Sprite3D::camera = camera;
	}
	//親スプライトをセット
	void SetParent(Sprite3D* parent) { this->parent = parent; }
protected:
	void CreateSprite3D();
	static Camera* camera;
	//シェーダ―用
	struct VERTEX
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 uv;
	};
	struct ConstBufferData
	{
		XMMATRIX viewprojection;
		XMMATRIX world;
		XMFLOAT3 cameraPos;
		float pad;//パディング
		XMFLOAT4 color;
	};


	float spriteRotation = 0.0f;
	XMMATRIX spritePosition{};
	XMMATRIX spriteMatScale{};
	XMMATRIX spriteMatWorld{};
	std::array<VERTEX, 4> vertices;
	std::array<unsigned short,6> indices;
	ComPtr<ID3D12Resource> vertBuff;//頂点バッファ
	ComPtr<ID3D12Resource> indexBuff;
	ComPtr<ID3D12Resource> constBuff;//定数バッファ
	ComPtr<ID3D12Resource> constBuff2;//定数バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView{};//頂点バッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView{};

	bool rect = false;//切り出しをしているかどうか
	XMFLOAT2 rectTexSize;

	//親スプライト
	Sprite3D* parent = nullptr;
};