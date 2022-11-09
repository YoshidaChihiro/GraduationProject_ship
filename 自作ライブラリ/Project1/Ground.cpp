#include "Ground.h"
#include "OBJLoader.h"

Ground::Ground(const Vector3& arg_pos, const Vector3& arg_scale)
{
	//�A�j���[�V�����p�Ƀ��f���̃|�C���^���i�[
	myModel = OBJLoader::GetModel("box");
	//���f���̐���
	Create(myModel);

	name = typeid(*this).name();

	position = arg_pos;
	scale = arg_scale;

	Initialize();
}

Ground::~Ground()
{
}

void Ground::Initialize()
{
	color = { 0.0f,0.5f,1.0f,1 };
	hitBox.SetPosition(position);
	hitBox.SetScale(scale);
}

void Ground::Update()
{
	Object::Update();
}

void Ground::Draw()
{
	Object::CustomDraw(false, true);
}

void Ground::DrawReady()
{
	pipelineName = "BasicObj";
}
