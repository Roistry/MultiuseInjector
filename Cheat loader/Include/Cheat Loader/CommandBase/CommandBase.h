#pragma once

class CommandBase
{
public:
	char* callMessage;

	virtual void OnCall(char* message) = 0;
};