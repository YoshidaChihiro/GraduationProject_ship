#include "CourseObstacle.h"
#include "OBJLoader.h"

CourseObstacle::CourseObstacle(const Vector3& arg_pos, const Vector3& arg_scale)
{
	//アニメーション用にモデルのポインタを格納
	myModel = OBJLoader::GetModel("box");
	//モデルの生成
	Create(myModel);

	name = typeid(*this).name();

	position = arg_pos;
	scale = arg_scale;

	Initialize();
}

CourseObstacle::~CourseObstacle()
{
}

void CourseObstacle::Initialize()
{
	color = { 0.7f,0.0f,0.0f,1 };
}

void CourseObstacle::Update()
{
	Object::Update();
}

void CourseObstacle::Draw()
{
	Object::CustomDraw(false, true);
}

void CourseObstacle::DrawReady()
{
	pipelineName = "BasicObj";
}
