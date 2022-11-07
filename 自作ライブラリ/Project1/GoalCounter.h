#pragma once

class GoalCounter
{
public:
	GoalCounter();
	~GoalCounter();

	void Initialize(const int arg_limit);

	void Add();

	int GetCount();

	bool GetEnd();

private:
	int limit = 0;
	int count = 0;
};

