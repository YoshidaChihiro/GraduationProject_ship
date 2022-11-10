#include "GoalSquare.h"
#include "OBJLoader.h"

GoalSquare::GoalSquare(const Vector3& arg_pos, const Vector3& arg_scale_hitBox)
{
	//アニメーション用にモデルのポインタを格納
	myModel = OBJLoader::GetModel("goal");
	//モデルの生成
	Create(myModel);

	name = typeid(*this).name();

	position = arg_pos;
	hitBox.SetScale(arg_scale_hitBox);

	Initialize();
}

GoalSquare::~GoalSquare()
{
}

void GoalSquare::Initialize()
{
	scale = Vector3(0.55f,0.4f,1) * hitBox.GetScale();
	hitBox.SetPosition(position);
}

void GoalSquare::Update()
{
	Object::Update();
}

void GoalSquare::Draw()
{
	Object::CustomDraw(false, true);
}

void GoalSquare::DrawReady()
{
	pipelineName = "BasicObj";
}
