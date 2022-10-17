#pragma once
#include <forward_list>

#include "Object.h"

class LevelEditor
{
public:
	static LevelEditor* GetInstance();
	void Draw();
	void Clear();
	void Delete();
private:
	void Save(const int slotNum);
	void Load(const int slotNum);
	LevelEditor() = default;
	LevelEditor(const LevelEditor&) = delete;
	~LevelEditor() = default;
	LevelEditor& operator=(const LevelEditor&) = delete;
	std::shared_ptr<Object> nowSelectObject = nullptr;
	std::forward_list<std::shared_ptr<Object>> objects;
	bool onDelete = false;
	int deleteCounter = 0;
	float length = 1.0f;

};

