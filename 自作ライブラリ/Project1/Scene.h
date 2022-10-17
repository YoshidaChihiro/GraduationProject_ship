#pragma once
class Scene
{
public:
	enum SCENE
	{
		Title,
		Play,
		Ending,
		Test,
		ParticleTestScene,
	};
	Scene() {};
	virtual ~Scene() {};
	//初期化
	virtual void Initialize() = 0;
	//更新
	virtual void Update() = 0;

	//背面描画
	virtual void PreDraw() = 0;
	//前面描画
	virtual void PostDraw() = 0;//仮にPostDrawにスプライト描画を入れるとオブジェクトの上にスプライトが描画される


	//終了フラグを取得
	inline bool GetIsEnd()
	{
		return isEnd;
	}
	//次のシーンに進む
	inline SCENE NextScene()
	{
		return next;
	}

protected:
	//終了フラグをTrueにする
    inline void ShutDown()
	{
		isEnd = true;
	}
	//終了フラグ
	bool isEnd;
	//シーン遷移開始フラグ
	bool nextFeed;
	//次のシーン
	SCENE next;
};

