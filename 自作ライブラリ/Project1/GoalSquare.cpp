#include "GoalSquare.h"
#include "FBXManager.h"

GoalSquare::GoalSquare(const Vector3& arg_pos, const Vector3& arg_scale)
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

GoalSquare::~GoalSquare()
{
}

void GoalSquare::Initialize()
{
	color = {0.7f,0.0f,0.0f,1};
}

void GoalSquare::Update()
{
	Object::Update();
}

void GoalSquare::Draw()
{
	Object::CustomDraw(true, true);
}

void GoalSquare::DrawReady()
{
	pipelineName = "FBX";
}
