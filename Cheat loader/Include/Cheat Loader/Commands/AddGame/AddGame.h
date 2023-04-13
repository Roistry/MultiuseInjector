#pragma once

#include "Cheat Loader/CommandBase/CommandBase.h"
#include "Cheat Loader/GameManager/GameManager.h"
#include "Cheat Loader/Console/Console.h"

class AddGame : public CommandBase
{
public:
	GameManager* gameManager;
	bool openGUI;

	AddGame(GameManager* gameManager);
	virtual void OnCall(char* message);
};