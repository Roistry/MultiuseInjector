#pragma once
#include <vector>

#include "Cheat Loader/CommandBase/CommandBase.h"
#include "Cheat Loader/Console/Console.h"

class Clear : public CommandBase
{
public:
	Clear();
	virtual void OnCall(char* message);
};