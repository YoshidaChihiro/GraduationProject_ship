#include "CourseSquare.h"
#include "FBXManager.h"

CourseSquare::CourseSquare(const Vector3& arg_pos, const Vector3& arg_scale)
{
	//�A�j���[�V�����p�Ƀ��f���̃|�C���^���i�[
	myModel = FBXManager::GetModel("box");
	//���f���̐���
	Create(myModel);

	name = typeid(*this).name();

	position = arg_pos;
	scale = arg_scale;

	Initialize();
}

CourseSquare::~CourseSquare()
{
}

void CourseSquare::Initialize()
{
}

void CourseSquare::Update()
{
	Object::Update();
}

void CourseSquare::Draw()
{
	Object::CustomDraw(true, true);
}

void CourseSquare::DrawReady()
{
	pipelineName = "FBX";
}
