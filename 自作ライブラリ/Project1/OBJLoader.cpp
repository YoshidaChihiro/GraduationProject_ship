#include "OBJLoader.h"

std::unordered_map<std::string, std::unique_ptr<OBJModel>> OBJLoader::models = {};

void OBJLoader::LoadModelFile(const std::string & keyName, const std::string & fileName, const bool& smooth)
{
	OBJModel* model = OBJModel::CreateOBJ(fileName, smooth);

	models[keyName].reset(model);
}
