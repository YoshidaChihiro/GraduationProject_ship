#pragma once
#include"ParticleParameter.h"
#include<d3d12.h>
class Particle
{
public:
	ParticleParameter parameter;
	OutputData vsParam;

	Particle(){};
	virtual ~Particle() {};
	void Dead() { vsParam.isDead = true; }
	bool GetisDead(){ return vsParam.isDead; }
};

