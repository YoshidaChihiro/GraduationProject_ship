#include "CourseSquare.h"
#include "OBJLoader.h"

CourseSquare::CourseSquare(const int arg_csvNum, const Vector3& arg_pos, const Vector3& arg_scale_hitBox)
{
	//CSVÇÃî‘çÜÇ…ÇÊÇ¡Çƒå¸Ç´Ç‚ÉÇÉfÉãÇïœçX
	ModelChange_csv(arg_csvNum);

	//ÉÇÉfÉãÇÃê∂ê¨
	myModel = OBJLoader::GetModel(modelName);
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

void CourseSquare::ModelChange_csv(const int arg_csvNum)
{
	switch (arg_csvNum)
	{
	case 1:
		modelName = "wall_flat";
		rotation.y = 0.0f;
		break;

	case 2:
		modelName = "wall_flat";
		rotation.y = 90.0f;
		break;

	case 3:
		modelName = "wall_flat";
		rotation.y = 180.0f;
		break;

	case 4:
		modelName = "wall_flat";
		rotation.y = 270.0f;
		break;

	case 5:
		modelName = "wall_corner";
		rotation.y = 0.0f;
		break;

	case 6:
		modelName = "wall_corner";
		rotation.y = 90.0f;
		break;

	case 7:
		modelName = "wall_corner";
		rotation.y = 180.0f;
		break;

	case 8:
		modelName = "wall_corner";
		rotation.y = 270.0f;
		break;

	default:
		break;
	}
}
