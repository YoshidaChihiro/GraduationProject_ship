//#include "ColliderEditor.h"
//#include<fstream>
//#include <memory>
//#include <sstream>
//#include "Chair.h"
//#include "Desk.h"
//#include "TeacherDesk.h"
//
//ColliderEditor* ColliderEditor::GetInstance()
//{
//	static ColliderEditor instance;
//	return &instance;
//}
//
//void ColliderEditor::Update()
//{
//	auto end_it = objects.end();
//	for (auto it = objects.begin(); it != end_it; ++it)
//	{
//		(*it)->Update();
//	}
//}
//
//void ColliderEditor::Draw()
//{
//	ImGui::Begin("LevelEditor", nullptr, ImGuiWindowFlags_MenuBar);
//	//ImGui::Begin("LevelEditor");
//	if (ImGui::BeginMenuBar()) {
//		if (ImGui::BeginMenu("File"))
//		{
//			if (ImGui::BeginMenu("Save"))
//			{
//				for (int i = 0; i < 10; i++)
//				{
//					std::string name = "Slot" + std::to_string(i + 1);
//					if (ImGui::MenuItem(name.c_str()))
//					{
//						Save(i);
//					}
//				}
//				ImGui::EndMenu();
//			}
//			if (ImGui::BeginMenu("Load"))
//			{
//				for (int i = 0; i < 10; i++)
//				{
//					std::string name = "Slot" + std::to_string(i + 1);
//					if (ImGui::MenuItem(name.c_str()))
//					{
//						Load(i);
//					}
//				}
//				ImGui::EndMenu();
//			}
//			ImGui::EndMenu();
//		}
//		//ImGui::SetNextTreeNodeOpen(true);
//		if (ImGui::BeginMenu("AddObject"))
//		{
//			if (ImGui::Button("Chair"))
//			{
//				std::shared_ptr<Object> ptr(new Chair());
//				objects.push_front(ptr);
//			}
//			if (ImGui::Button("Desk"))
//			{
//				std::shared_ptr<Object> ptr(new Desk());
//				objects.push_front(ptr);
//			}
//			if (ImGui::Button("TeacherDesk"))
//			{
//				std::shared_ptr<Object> ptr(new TeacherDesk());
//				objects.push_front(ptr);
//			}
//
//			ImGui::EndMenu();
//		}
//		ImGui::EndMenuBar();
//	}
//
//	int i = 0;
//	for (auto it = objects.begin(); it != objects.end(); ++it)
//	{
//		std::string name = typeid(**it).name() + std::to_string(i);
//		if (ImGui::Button(name.c_str()))
//		{
//			if (nowSelectObject != nullptr)
//				nowSelectObject->SetColor({ 1,1,1,1 });
//			nowSelectObject = *it;
//			nowSelectObject->SetColor({ 1,0.3f,0.3f,1 });
//		}
//		i++;
//	}
//	ImGui::End();
//	if (nowSelectObject == nullptr)
//		return;
//
//	//std::string name = typeid(*nowSelectObject).name() ;
//	//name += "State";
//	ImGui::Begin("SelectObjectState");
//	float pos[3], scale[3], rot[3];
//	memcpy(pos, &nowSelectObject->GetPosition(), sizeof(float) * 3);
//	memcpy(scale, &nowSelectObject->GetScale(), sizeof(float) * 3);
//	memcpy(rot, &nowSelectObject->GetRotation(), sizeof(float) * 3);
//	ImGui::SliderFloat("SliderLength", &length, 0, 20);
//	ImGui::Text("position");
//	ImGui::SliderFloat("position_x", &pos[0], -10 * length + pos[0], 10 * length + pos[0]);
//	ImGui::SliderFloat("position_y", &pos[1], -10 * length + pos[1], 10 * length + pos[1]);
//	ImGui::SliderFloat("position_z", &pos[2], -10 * length + pos[2], 10 * length + pos[2]);
//	ImGui::Text("scale");
//	ImGui::SliderFloat("scale_x", &scale[0], -5 * length + scale[0], 5 * length + scale[0]);
//	ImGui::SliderFloat("scale_y", &scale[1], -5 * length + scale[1], 5 * length + scale[1]);
//	ImGui::SliderFloat("scale_z", &scale[2], -5 * length + scale[2], 5 * length + scale[2]);
//	//ImGui::SliderFloat3("scale", scale, 0, 5 * length);
//	ImGui::Text("rotation");
//	ImGui::SliderFloat("rotation_x", &rot[0], -10 * length + rot[0], 10 * length + rot[0]);
//	ImGui::SliderFloat("rotation_y", &rot[1], -10 * length + rot[1], 10 * length + rot[1]);
//	ImGui::SliderFloat("rotation_z", &rot[2], -10 * length + rot[2], 10 * length + rot[2]);
//	//ImGui::SliderFloat3("rot", rot, 0, 360);
//	if (ImGui::Button("Delete")) {
//		//deleteLists.push_front(nowSelectObject);
//		onDelete = true;
//		objects.remove(nowSelectObject);
//	}
//	else
//	{
//		nowSelectObject->SetPosition(pos);
//		nowSelectObject->SetScale(scale);
//		nowSelectObject->SetRotation(rot);
//	}
//	ImGui::End();
//
//}
//
//void ColliderEditor::ObjectDraw()
//{
//	auto end_it = objects.end();
//	for (auto it = objects.begin(); it != end_it; ++it)
//	{
//		(*it)->Draw();
//	}
//
//
//}
//
//void ColliderEditor::Clear()
//{
//	//Save();
//	auto end_it = objects.end();
//	for (auto it = objects.begin(); it != end_it; ++it)
//	{
//		(*it).reset();
//	}
//	objects.clear();
//
//}
//
//void ColliderEditor::Delete()
//{
//	if (!onDelete)
//		return;
//	if (deleteCounter >= 2)
//	{
//		onDelete = false;
//		nowSelectObject.reset();
//		deleteCounter = 0;
//	}
//	deleteCounter++;
//
//}
//
//void ColliderEditor::Save(const int slotNum)
//{
//	std::string filepath = "Resources/Map/Report" + std::to_string(slotNum) + ".txt";
//	//ファイル入力ストリームのインスタンス
//	std::ofstream file;
//	//.wavファイルをバイナリモードで開く
//	file.open(filepath.c_str(), std::ios_base::binary);
//	assert(file.is_open());
//	//std::string size = "size : " + std::to_string(objects.max_size()) + "\n";
//	//file.write(size.c_str(), size.length());
//
//	auto end_it = objects.end();
//	for (auto it = objects.begin(); it != end_it; ++it)
//	{
//		std::string name = typeid(**it).name();
//		name += "\n";
//		file.write(name.c_str(), name.length());
//		Vector3 position = (*it)->GetPosition();
//		std::string write = "position ";
//		write += std::to_string(position.x) + " " + std::to_string(position.y) + " " + std::to_string(position.z) + "\n";
//		file.write(write.c_str(), write.length());
//		Vector3 scale = (*it)->GetScale();
//		write = "scale ";
//		write += std::to_string(scale.x) + " " + std::to_string(scale.y) + " " + std::to_string(scale.z) + "\n";
//		file.write(write.c_str(), write.length());
//		Vector3 rot = (*it)->GetRotation();
//		write = "rotation ";
//		write += std::to_string(rot.x) + " " + std::to_string(rot.y) + " " + std::to_string(rot.z) + "\n";
//		file.write(write.c_str(), write.length());
//	}
//	file.close();
//
//}
//
//void ColliderEditor::Load(const int slotNum)
//{
//	Clear();
//	std::string filepath = "Resources/Map/Report" + std::to_string(slotNum) + ".txt";
//	std::ifstream file;
//	file.open(filepath.c_str());
//	if (!file.is_open())
//		return;
//	std::string line;
//	std::vector<Vector3>positions;
//	std::vector<Vector3>scales;
//	std::vector<Vector3>rotations;
//	std::vector<std::shared_ptr<Object>>loadObjects;
//
//	while (getline(file, line))
//	{
//		//1行分の文字列をストリームに変換して解析しやすくする
//		std::istringstream line_stream(line);
//
//		//半角スペース区切りで行の先頭文字を取得
//		std::string key;
//		getline(line_stream, key, ' ');
//
//		//クラス名
//		if (key == "class")
//		{
//			std::string name;
//			line_stream >> name;
//			if (name == "Chair")
//			{
//				std::shared_ptr<Object> ptr(new Chair());
//				loadObjects.push_back(ptr);
//			}
//			else if (name == "Desk")
//			{
//				std::shared_ptr<Object> ptr(new Desk());
//				loadObjects.push_back(ptr);
//			}
//			else if (name == "TeacherDesk")
//			{
//				std::shared_ptr<Object> ptr(new TeacherDesk());
//				loadObjects.push_back(ptr);
//			}
//
//		}
//		//ポジション
//		if (key == "position")
//		{
//			Vector3 position;
//			line_stream >> position.x;
//			line_stream >> position.y;
//			line_stream >> position.z;
//
//			positions.push_back(position);
//		}
//		//スケール
//		if (key == "scale")
//		{
//			Vector3 scale;
//			line_stream >> scale.x;
//			line_stream >> scale.y;
//			line_stream >> scale.z;
//
//			scales.push_back(scale);
//		}
//		//回転
//		if (key == "rotation")
//		{
//			Vector3 rotation;
//			line_stream >> rotation.x;
//			line_stream >> rotation.y;
//			line_stream >> rotation.z;
//
//			rotations.push_back(rotation);
//		}
//	}
//	file.close();
//
//	int i = 0;
//	auto endIt = loadObjects.end();
//	for (auto it = loadObjects.begin(); it != endIt; ++it)
//	{
//		(*it)->SetPosition(positions[i]);
//		(*it)->SetScale(scales[i]);
//		(*it)->SetRotation(rotations[i]);
//		objects.push_front(*it);
//		i++;
//	}
//
//}
