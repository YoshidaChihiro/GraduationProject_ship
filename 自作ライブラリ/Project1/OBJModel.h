#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Mesh.h"
#include"Model.h"
class OBJModel:public Model
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
public://静的メンバ関数
	static OBJModel* CreateOBJ(const std::string& name, bool smooth);

private://静的メンバ変数
	static const std::string basePath;

public://メンバ関数
	~OBJModel();

	void Initialize(const std::string& name, bool smooth);
	void Draw()override;
	void LoadMaterial(const std::string& name);
	void AddMaterial(Material* material);
	void Loadtextures();
	void CreateDescriptorHeap();

	//メッシュコンテナを取得
	inline const std::vector<Mesh*>& GetMeshes() { return meshes; }
	//マテリアルコンテナを取得
	inline const std::unordered_map<std::string, Material*>& GetMaterials() { return materials; }

private://メンバ変数
	std::string name;
	std::vector<Mesh*>meshes;
	std::unordered_map<std::string, Material*>materials;
	Material* defaultMaterial = nullptr;
	ComPtr<ID3D12DescriptorHeap> descHeap;

};

