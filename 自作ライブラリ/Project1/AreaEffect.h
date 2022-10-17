#pragma once
#include "Object.h"
class AreaEffect :
    public Object
{
public:
	AreaEffect(const Vector3& pos, const XMFLOAT4& arg_color);
	void Initialize() override;
	void Update() override;
	void Draw();
	static void AreaEffectsDraw();
private:
	void EffectDraw();
	static std::vector<AreaEffect*> effects;
	struct ConstBuffDeta
	{
		XMFLOAT2 offsetTime = { 0,0 };
		XMFLOAT2 pad;
	};
	float offsetTime;
	ComPtr<ID3D12Resource> constBuff; // 定数バッファ

};
