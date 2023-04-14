#pragma once

#include "Cheat Loader/Console/CommandBase/CommandBase.h"
#include "Cheat Loader/Game/GameManager/GameManager.h"
#include "Cheat Loader/Console/Console.h"

class AddGame : public CommandBase
{
public:
	GameManager* gameManager;
	bool openGUI;

	AddGame(GameManager* gameManager);
	virtual void OnCall(char* message);
};