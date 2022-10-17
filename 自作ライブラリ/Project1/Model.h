#pragma once
class Model
{
public:
	Model() = default;
	virtual ~Model(){};
	virtual void Draw() = 0;
	
};

