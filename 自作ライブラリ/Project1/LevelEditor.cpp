#include "LevelEditor.h"
#include<fstream>
#include <memory>
#include <sstream>

#include "ObjectManager.h"

LevelEditor* LevelEditor::GetInstance()
{
	static LevelEditor instance;
	return &instance;
}

void LevelEditor::Draw()
{
	ImGui::Begin("LevelEditor", nullptr, ImGuiWindowFlags_MenuBar);
	//メニューバー内の処理
	if (ImGui::BeginMenuBar()) {
		//ファイルタブ
		if (ImGui::BeginMenu("File"))
		{
			//セーブ
			if (ImGui::BeginMenu("Save"))
			{
				for (int i = 0; i < 10; i++)
				{
					std::string name = "Slot" + std::to_string(i + 1);
					if (ImGui::MenuItem(name.c_str()))
					{
						Save(i);
					}
				}
				ImGui::EndMenu();
			}
			//ロード
			if (ImGui::BeginMenu("Load"))
			{
				for (int i = 0; i < 10; i++)
				{
					std::string name = "Slot" + std::to_string(i + 1);
					if (ImGui::MenuItem(name.c_str()))
					{
						Load(i);
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		//コライダー追加タブ
		if (ImGui::BeginMenu("AddObject"))
		{
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	int i = 0;
	//オブジェクト一覧表示
	for (auto& it : objects)
	{
		std::string name = typeid(*it).name() + std::to_string(i);
		if (ImGui::Button(name.c_str()))
		{
			if (nowSelectObject != nullptr)
				nowSelectObject->SetColor({ 1,1,1,1 });
			nowSelectObject = it;
			nowSelectObject->SetColor({ 1,0.3f,0.3f,1 });
		}
		i++;
	}
	ImGui::End();
	if (nowSelectObject == nullptr)
		return;

	//選択中のオブジェクトについて
	ImGui::Begin("SelectObjectState");
	float pos[3], scale[3], rot[3];
	//ポジション、スケール、ローテーションを取得
	memcpy(pos, &nowSelectObject->GetPosition(), sizeof(float) * 3);
	memcpy(scale, &nowSelectObject->GetScale(), sizeof(float) * 3);
	memcpy(rot, &nowSelectObject->GetRotation(), sizeof(float) * 3);
	ImGui::SliderFloat("SliderLength", &length, 0, 20);
	//ポジション
	ImGui::Text("position");
	ImGui::SliderFloat("position_x", &pos[0], -10 * length + pos[0], 10 * length + pos[0]);
	ImGui::SliderFloat("position_y", &pos[1], -10 * length + pos[1], 10 * length + pos[1]);
	ImGui::SliderFloat("position_z", &pos[2], -10 * length + pos[2], 10 * length + pos[2]);
	//スケール
	ImGui::Text("scale");
	ImGui::SliderFloat("scale_x", &scale[0], -5 * length + scale[0], 5 * length + scale[0]);
	ImGui::SliderFloat("scale_y", &scale[1], -5 * length + scale[1], 5 * length + scale[1]);
	ImGui::SliderFloat("scale_z", &scale[2], -5 * length + scale[2], 5 * length + scale[2]);
	//ローテーション
	ImGui::Text("rotation");
	ImGui::SliderFloat("rotation_x", &rot[0], -10 * length + rot[0], 10 * length + rot[0]);
	ImGui::SliderFloat("rotation_y", &rot[1], -10 * length + rot[1], 10 * length + rot[1]);
	ImGui::SliderFloat("rotation_z", &rot[2], -10 * length + rot[2], 10 * length + rot[2]);
	//コピー項目
	if (ImGui::Button("Copy")) {
		//選択中のオブジェクトのクラス名を取得
		std::string name = typeid(*nowSelectObject).name();
		std::shared_ptr<Object> ptr = nullptr;
		//オブジェクトが作成されていたら
		if (ptr != nullptr)
		{
			//ポジション、スケール、ローテーションを設定
			ptr->SetPosition(nowSelectObject->GetPosition());
			ptr->SetScale(nowSelectObject->GetScale());
			ptr->SetRotation(nowSelectObject->GetRotation());
			ptr->Update();
			objects.push_front(ptr);
			//オブジェクトマネージャーに追加
			ObjectManager::GetInstance()->Add(ptr.get());

		}
	}

	//削除ボタンが押されたら
	if (ImGui::Button("Delete")) {
		//削除フラグをONに
		onDelete = true;
		//オブジェクトマネージャーか削除
		ObjectManager::GetInstance()->Remove(nowSelectObject.get());
		//エディタ内のオブジェクトリストからも削除
		objects.remove(nowSelectObject);
	}
	else
	{
		//ポジション、スケール、ローテーションの更新
		nowSelectObject->SetPosition(pos);
		nowSelectObject->SetScale(scale);
		nowSelectObject->SetRotation(rot);
	}
	ImGui::End();
	
}

void LevelEditor::Clear()
{
	nowSelectObject.reset();
	for (auto& it : objects)
	{
		ObjectManager::GetInstance()->Remove(it.get());
		it.reset();
	}
	objects.clear();
}

void LevelEditor::Delete()
{
	//削除フラグが立っていなかったら早期リターン
	if(!onDelete)
		return;
	//2フレーム待機後削除
	if(deleteCounter>=2)
	{
		ObjectManager::GetInstance()->Remove(nowSelectObject.get());
		onDelete = false;
		nowSelectObject.reset();
		deleteCounter = 0;
	}
	deleteCounter++;
	
}

void LevelEditor::Save(const int slotNum)
{
	//ファイルパスの指定
	std::string filepath = "Resources/Map/Report"+std::to_string(slotNum)+".txt";
	//ファイル入力ストリームのインスタンス
	std::ofstream file;
	//.wavファイルをバイナリモードで開く
	file.open(filepath.c_str(), std::ios_base::binary);
	assert(file.is_open());
	for (auto& it : objects)
	{
		//クラス名の取得
		std::string name = typeid(*it).name();
		name += "\n";
		file.write(name.c_str(), name.length());//ファイルに書き込む
		//ポジションの取得
		Vector3 position = it->GetPosition();
		std::string write = "position ";
		write += std::to_string(position.x) + " " + std::to_string(position.y) + " " + std::to_string(position.z)+"\n";
		file.write(write.c_str(), write.length());//ファイルに書き込む
		//スケールの取得
		Vector3 scale = it->GetScale();
		write = "scale ";
		write += std::to_string(scale.x) + " " + std::to_string(scale.y) + " " + std::to_string(scale.z) + "\n";
		file.write(write.c_str(), write.length());//ファイルに書き込む
		//ローテーションの取得
		Vector3 rot = it->GetRotation();
		write = "rotation ";
		write += std::to_string(rot.x) + " " + std::to_string(rot.y) + " " + std::to_string(rot.z) + "\n";
		file.write(write.c_str(), write.length());//ファイルに書き込む
	}
	file.close();

}

void LevelEditor::Load(const int slotNum)
{
	Clear();
	std::string filepath = "Resources/Map/Report" + std::to_string(slotNum) + ".txt";
	std::ifstream file;
	file.open(filepath.c_str());
	if(!file.is_open())
		return;
	std::string line;
	std::vector<Vector3>positions;
	std::vector<Vector3>scales;
	std::vector<Vector3>rotations;
	std::vector<std::shared_ptr<Object>>loadObjects;

	while (getline(file, line))
	{
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		//半角スペース区切りで行の先頭文字を取得
		std::string key;
		getline(line_stream, key, ' ');

		//クラス名
		if (key == "class")
		{
			std::string name;
			line_stream >> name;
		}
		//ポジション
		if (key == "position")
		{
			Vector3 position;
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;

			positions.push_back(position);
		}
		//スケール
		if (key == "scale")
		{
			Vector3 scale;
			line_stream >> scale.x;
			line_stream >> scale.y;
			line_stream >> scale.z;

			scales.push_back(scale);
		}
		//回転
		if (key == "rotation")
		{
			Vector3 rotation;
			line_stream >> rotation.x;
			line_stream >> rotation.y;
			line_stream >> rotation.z;

			rotations.push_back(rotation);
		}
	}
	file.close();

	int i = 0;
	for (auto& it : loadObjects)
	{
		it->SetPosition(positions[i]);
		it->SetScale(scales[i]);
		it->SetRotation(rotations[i]);
		objects.push_front(it);
		ObjectManager::GetInstance()->Add(it.get());
		i++;
	}
	
}
