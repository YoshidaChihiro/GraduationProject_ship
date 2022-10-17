#include "ConsoleWindow.h"
#include <Windows.h>
#pragma comment(lib, "winmm.lib")

ConsoleWindow::ConsoleWindow()
{
}

ConsoleWindow::~ConsoleWindow()
{
	delete fp;
}

void ConsoleWindow::CreateConsoleWindow()
{
	AllocConsole();
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "COIN$", "r", stdin);
}
