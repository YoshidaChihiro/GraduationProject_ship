#include "OBJModel.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include"Texture.h"
#include"DirectXLib.h"
using namespace std;

const std::string OBJModel::basePath = "Resources/OBJ/";

OBJModel * OBJModel::CreateOBJ(const std::string & name, bool smooth)
{
	OBJModel* instance = new OBJModel;
	instance->Initialize(name, smooth);
	return instance;
}

OBJModel::~OBJModel()
{
	for (auto m : meshes)
	{
		delete m;
	}
	meshes.clear();

	for (auto m : materials)
	{
		delete m.second;
	}
	materials.clear();
}

void OBJModel::Initialize(const std::string & name, bool smooth)
{
	std::ifstream file;
	file.open(basePath + name);
	if (file.fail())
	{
		assert(0);
	}

	this->name = name;

	Mesh* mesh = new Mesh;
	int indexCountTex = 0;
	int indexCountNoTex = 0;

	vector<XMFLOAT3>positions;//頂点座標
	vector<XMFLOAT3>normals;//法線ベクトル
	vector<XMFLOAT2>texcoords;//テクスチャUV

	//1行ずつ読み込む
	string line;
	while (getline(file,line))
	{
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		//半角スペース区切りで行の先頭文字を取得
		string key;
		getline(line_stream, key, ' ');

		//マテリアル
		if (key == "mtllib")
		{
			string filename;
			line_stream >> filename;
			LoadMaterial(filename);
		}
		//先頭文字列がgならグループの開始
		if (key == "g" && positions.size() > 0)
		{
			if (mesh->GetName().size() > 0)
			{
				//平滑化
				if (smooth)
				{
					mesh->CalculateSmoothedVertexNormals();
				}
				meshes.emplace_back(mesh);
				mesh = new Mesh;
				indexCountTex = 0;
			}
			
			string groupName;
			line_stream >> groupName;

			//メッシュに名前をセット
			mesh->SetName(groupName);
		}

		//先頭文字列がvなら頂点座標
		if (key == "v")
		{
			XMFLOAT3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			positions.emplace_back(position);
		}
		//先頭文字列がvtならテクスチャ
		if (key == "vt")
		{
			XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			//V方向反転
			texcoord.y = 1.0f - texcoord.y;
			//テクスチャ座標データに追加
			texcoords.emplace_back(texcoord);
		}
		//先頭文字列がvnなら法線ベクトル
		if (key == "vn")
		{
			XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			normals.emplace_back(normal);
		}
		//先頭文字列がusemtlならマテリアルを割り当てる
		if (key == "usemtl")
		{
			if (mesh->GetMaterial() == nullptr)
			{
				//マテリアルの
				string materialName;
				line_stream >> materialName;

				auto itr = materials.find(materialName);
				if (itr != materials.end())
				{
					mesh->SetMaterial(itr->second);
				}
			}
		}
		//先頭文字列がfならポリゴン(三角形)
		if (key == "f")
		{
			int faceIndexCount = 0;
			//半角スペース区切りで行の続きを読み込む
			string index_string;
			while (getline(line_stream,index_string,' '))
			{
				//頂点インデックス1個分の文字列をストリームに変換して解析しやすくする
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				//頂点番号
				index_stream >> indexPosition;

				Material* material = mesh->GetMaterial();
				index_stream.seekg(1, ios_base::cur);
				
				if (material && material->textureFileName.size() > 0)
				{
					index_stream >> indexTexcoord;
					index_stream.seekg(1, ios_base::cur);
					index_stream >> indexNormal;
					//頂点データの追加
					Mesh::VERTEXSTATUS vertex{};
					vertex.pos = positions[indexPosition - 1];
					vertex.normal = normals[indexNormal - 1];
					vertex.uv = texcoords[indexTexcoord - 1];
					mesh->AddVertex(vertex);
					//エッジ平滑化用のデータを追加
					if (smooth)
					{
						//vキー(座標データ)の番号と、全て合成した頂点のインデックスをセットで登録
						mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
					}
				}
				else
				{
					char c;
					index_stream >> c;
					//スラッシュ2連続の場合、頂点番号のみ
					if (c == '/')
					{
						//頂点データの追加
						Mesh::VERTEXSTATUS vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = { 0,0,1 };
						vertex.uv = { 0,0 };
						mesh->AddVertex(vertex);
						//エッジ平滑化用のデータを追加
						if (smooth)
						{
							mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
						}
					}
					else
					{
						index_stream.seekg(-1, ios_base::cur);
						index_stream >> indexTexcoord;
						index_stream.seekg(1, ios_base::cur);
						index_stream >> indexNormal;
						//頂点データの追加
						Mesh::VERTEXSTATUS vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = normals[indexNormal - 1];
						vertex.uv = { 0,0 };
						mesh->AddVertex(vertex);
						if (smooth)
						{
							mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
						}
					}
				}
				if (faceIndexCount >= 3)
				{
					mesh->AddIndex(indexCountTex - 1);
					mesh->AddIndex(indexCountTex);
					mesh->AddIndex(indexCountTex - 3);
				}
				else
				{
					mesh->AddIndex(indexCountTex);
				}
				indexCountTex++;
				faceIndexCount++;
			}

		}
	}
	file.close();

	if (smooth)
	{
		mesh->CalculateSmoothedVertexNormals();
	}
	//コンテナに登録
	meshes.emplace_back(mesh);	
	//メッシュのマテリアルチェック
	for (auto& m : meshes)
	{
		if (m->GetMaterial() == nullptr)
		{
			if (defaultMaterial == nullptr)
			{
				//デフォルトマテリアルを生成
				defaultMaterial = Material::Create();
				defaultMaterial->name = "no material";
				materials.emplace(defaultMaterial->name, defaultMaterial);
			}
			//デフォルトマテリアルをセット
			m->SetMaterial(defaultMaterial);
		}
	}

	//メッシュのバッファ生成
	for (auto& m : meshes)
	{
		m->CreateBuffers();
	}

	//マテリアルの数値を定数バッファに反映
	for (auto& m : materials)
	{
		m.second->Update();
	}
	// デスクリプタヒープ生成
	CreateDescriptorHeap();

	//テクスチャの読み込み
	Loadtextures();
}

void OBJModel::Draw()
{
	//ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	//DirectXLib::GetInstance()->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	for (auto& mesh : meshes)
	{
		mesh->Draw();
	}
	
}

void OBJModel::LoadMaterial(const std::string & name)
{
	//ファイルストリーム
	std::ifstream file;
	//マテリアルファイルを開く
	file.open(basePath + name);
	//ファイルオープン失敗をチェック
	if (file.fail())
	{
		assert(0);
	}

	Material* material = nullptr;

	//1行ずつ読み込む
	string line;
	while (getline(file,line))
	{
		std::istringstream line_stream(line);

		string key;
		getline(line_stream, key, ' ');

		//先頭のタブ文字は無視する
		if (key[0] == '\t')
		{
			key.erase(key.begin());
		}

		if (key == "newmtl")
		{
			if (material)
			{
				AddMaterial(material);
			}
			// 新しいマテリアルを生成
			material = Material::Create();
			// マテリアル名読み込み
			line_stream >> material->name;
		}
		// 先頭文字列がKaならアンビエント色
		if (key == "Ka") {
			line_stream >> material->ambient.x;
			line_stream >> material->ambient.y;
			line_stream >> material->ambient.z;
		}
		// 先頭文字列がKdならディフューズ色
		if (key == "Kd") {
			line_stream >> material->diffuse.x;
			line_stream >> material->diffuse.y;
			line_stream >> material->diffuse.z;
		}
		// 先頭文字列がKsならスペキュラー色
		if (key == "Ks") {
			line_stream >> material->specular.x;
			line_stream >> material->specular.y;
			line_stream >> material->specular.z;
		}
		// 先頭文字列がmap_Kdならテクスチャファイル名
		if (key == "map_Kd") {
			// テクスチャのファイル名読み込み
			line_stream >> material->textureFileName;

			// フルパスからファイル名を取り出す
			size_t pos1;
			pos1 = material->textureFileName.rfind('\\');
			if (pos1 != string::npos) {
				material->textureFileName = material->textureFileName.substr(pos1 + 1, material->textureFileName.size() - pos1 - 1);
			}

			pos1 = material->textureFileName.rfind('/');
			if (pos1 != string::npos) {
				material->textureFileName = material->textureFileName.substr(pos1 + 1, material->textureFileName.size() - pos1 - 1);
			}
		}
	}
	// ファイルを閉じる
	file.close();

	if (material) {
		// マテリアルを登録
		AddMaterial(material);
	}
}

void OBJModel::AddMaterial(Material * material)
{
	materials.emplace(material->name, material);
}

void OBJModel::CreateDescriptorHeap()
{
	HRESULT result = S_FALSE;

	// マテリアルの数
	size_t count = materials.size();
	// デスクリプタヒープを生成	
	if (count > 0) {
		D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
		descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
		descHeapDesc.NumDescriptors = (UINT)count + 1; // シェーダーリソースビューの数
		result = DirectXLib::GetInstance()->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));//生成
		if (FAILED(result)) {
			assert(0);
		}
	}
}

void OBJModel::Loadtextures()
{
	for (auto& m : materials)
	{
		Material* material = m.second;
		material->LoadTexture();
	}
}
