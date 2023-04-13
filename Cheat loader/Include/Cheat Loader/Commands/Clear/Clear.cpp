#include "Clear.h"

std::vector<char*> Console::messages;

Clear::Clear()
{
	this->callMessage = (char*)"Clear";
}

void Clear::OnCall(char* message)
{
	Console::messages.clear();
}