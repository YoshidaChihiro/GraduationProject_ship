#pragma once

#include "fbxsdk.h"
#include"FBXModel.h"
#include<string>

#include <d3d12.h>
#include <d3dx12.h>

class FbxLoader
{
private://エイリアス
	//std::を省略
	using string = std::string;
public://定数
	//モデル格納ルートパス
	static const string baseDirectory;

public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static FbxLoader* GetInstance();

	/// <summary>
	/// FBXの行列をXMMatrixに変換
	/// </summary>
	/// <param name = "dst">書き込み先</param>
	/// <param name = "src">元となるFBXの行列</param>
	static void ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src);
	
	void Initialize();

	void Finalize();

	//スキニング情報の読み取り
	void ParseSkin(FBXModel* model, FbxMesh* fbxMesh);
	
	/// <summary>
	/// ファイルからFBXモデル読み込み
	/// </summary>
	/// <param name = "modelName">モデル名</param>
	FBXModel* LoadModelFromFile(const string& modelName);

private:
	/// <summary>
	/// 再帰的にノード構成を解析
	/// </summary>
	/// <param name = "model">読み込み先モデルオブジェクト</param>
	/// <param name = "fbxNode">解析対象のノード</param>
	void ParseNodeRecursive(FBXModel* model, FbxNode* fbxNode,Node* parent = nullptr);

	/// <summary>
	/// メッシュの読み込み
	/// </summary>
	/// <param name = "model">読み込み先モデルオブジェクト</param>
	/// <param name = "fbxNode">解析対象のノード</param>
	void ParseMesh(FBXModel* model, FbxNode* fbxNode);

	
	//頂点座標読み取り
	void ParseMeshVertices(FBXModel* model, FbxMesh* fbxMesh);
	//面情報読み取り
	void ParseMeshFaces(FBXModel* model, FbxMesh* fbxMesh);
	//マテリアルの読み取り
	void ParseMaterial(FBXModel* model, FbxNode* fbxNode);
	//テクスチャの読み取り
	void LoadTexture(const std::string& fullpath);

//ディレクトリを含んだファイルパスからファイル名を抽出する
	std::string ExtractFileName(const std::string& path);
	
	// privateなコンストラクタ（シングルトンパターン）
	FbxLoader() = default;
	// privateなデストラクタ（シングルトンパターン）
	~FbxLoader() = default;
	// コピーコンストラクタを禁止（シングルトンパターン）
	FbxLoader(const FbxLoader& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FbxLoader& obj) = delete;
	//FBXマネージャー
	FbxManager* fbxManager = nullptr;
	//FBXインポータ―
	FbxImporter* fbxImporter = nullptr;

	//テクスチャがない場合の標準テクスチャファイル名
	static const string defaultTextureFileName;

};