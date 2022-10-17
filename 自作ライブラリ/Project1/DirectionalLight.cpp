#include "DirectionalLight.h"

void DirectionalLight::SetLightDir(const XMVECTOR & lightdir)
{
	this->lightdir = DirectX::XMVector3Normalize(lightdir);
}

void DirectionalLight::SetLightColor(const Vector3 & lightcolor)
{
	this->lightcolor = lightcolor;
	dirty = true;
}
