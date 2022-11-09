#include "CourseObstacle.h"
#include "OBJLoader.h"

CourseObstacle::CourseObstacle(const Vector3& arg_pos, const Vector3& arg_scale_hitBox)
{
	//�A�j���[�V�����p�Ƀ��f���̃|�C���^���i�[
	myModel = OBJLoader::GetModel("box");
	//���f���̐���
	Create(myModel);

	name = typeid(*this).name();

	position = arg_pos;
	hitBox.SetScale(arg_scale_hitBox);

	Initialize();
}

CourseObstacle::~CourseObstacle()
{
}

void CourseObstacle::Initialize()
{
	color = { 0.7f,0.0f,0.0f,1 };
	scale = Vector3(1, 1, 1) * hitBox.GetScale();
	hitBox.SetPosition(position);
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
