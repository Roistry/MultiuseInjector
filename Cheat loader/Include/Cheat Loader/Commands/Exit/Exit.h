#pragma once
#include <Windows.h>

#include "Cheat Loader/CommandBase/CommandBase.h"

class Exit : public CommandBase
{
public:
	Exit();
	virtual void OnCall(char* message);
};