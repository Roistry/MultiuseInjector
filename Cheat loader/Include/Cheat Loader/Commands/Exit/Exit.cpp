#include "Exit.h"

Exit::Exit()
{
	this->callMessage = (char*)"Exit";
}

void Exit::OnCall(char* message)
{
	HWND hwnd = GetForegroundWindow();
	PostMessageA(hwnd, WM_DESTROY, 0, 0);
}