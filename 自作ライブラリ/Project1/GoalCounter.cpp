#include "GoalCounter.h"

GoalCounter::GoalCounter()
{
}

GoalCounter::~GoalCounter()
{
}

void GoalCounter::Initialize(const int arg_limit)
{
	limit = arg_limit;
	count = 0;
}

void GoalCounter::Add()
{
	count++;
}

int GoalCounter::GetCount()
{
	return count;
}

bool GoalCounter::GetEnd()
{
	return count >= limit;
}
