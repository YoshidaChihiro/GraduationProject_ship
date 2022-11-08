#include "CourseSquare.h"
#include "OBJLoader.h"

CourseSquare::CourseSquare(const Vector3& arg_pos, const Vector3& arg_scale_hitBox)
{
	//アニメーション用にモデルのポインタを格納
	myModel = OBJLoader::GetModel("box");
	//モデルの生成
	Create(myModel);

	name = typeid(*this).name();

	position = arg_pos;
	hitBox.SetScale(arg_scale_hitBox);

	Initialize();
}

CourseSquare::~CourseSquare()
{
}

void CourseSquare::Initialize()
{
	scale = Vector3(1, 1, 1) * hitBox.GetScale();
	hitBox.SetPosition(position);
}

void CourseSquare::Update()
{
	Object::Update();
}

void CourseSquare::Draw()
{
	Object::CustomDraw(false, true);
}

void CourseSquare::DrawReady()
{
	pipelineName = "BasicObj";
}
