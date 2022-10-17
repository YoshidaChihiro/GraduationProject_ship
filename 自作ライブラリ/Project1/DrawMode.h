#pragma once
class DrawMode
{
public:
	enum MODE
	{
		Bloom,
		None,
	};
	static void SetMode(const MODE mode) { DrawMode::mode = mode; }
	inline static const MODE& GetBloomMode() { return mode; }
	inline static bool GetDrawImGui() { return drawImGui; }
	static void SetDrawImGui(const bool flag) { drawImGui = flag; }

private:
	static MODE mode;
	static bool drawImGui;
};

