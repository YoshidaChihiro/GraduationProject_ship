#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <memory>

#include"Scene.h"
#include<unordered_map>
#include <wrl/client.h>

class Sprite;
class TextureResource;


class SceneManager
{
public:
	static SceneManager* GetInstance();
	void Add(Scene::SCENE name,Scene* scene);
	void Initialize();
	void Update();
	void Change(Scene::SCENE next);
	void PreDraw();
	void PostDraw();
private:

	SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	~SceneManager() = default;
	SceneManager& operator=(const SceneManager&) = delete;
	struct ConstBufferDate
	{
		float migrateTime;
		DirectX::XMFLOAT3 pad;
	};

	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;//定数バッファ

	std::unordered_map<Scene::SCENE, std::unique_ptr<Scene>>scenes;
	Scene* currentScene = nullptr;
	std::unique_ptr<TextureResource> resource;
	std::unique_ptr<Sprite> migrateTex;
	bool migrateStart;
	float migrateTime;
	int migrateCounter;
};

