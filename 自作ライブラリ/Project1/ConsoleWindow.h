#pragma once
#include <fstream>

class ConsoleWindow
{
	FILE *fp = NULL;

public:
	ConsoleWindow();
	~ConsoleWindow();
	void CreateConsoleWindow();
};