#pragma once
#include "Scene.h"
#include "InGameCamera.h"
#include "LightGroup.h"

class Title : public Scene
{
public:
	Title();
	~Title() override;
	void Initialize() override;
	void Update() override;
	void PreDraw() override;
	void PostDraw() override;

private:
	std::unique_ptr<InGameCamera> camera;
	std::unique_ptr<LightGroup> lightGroup;

};
