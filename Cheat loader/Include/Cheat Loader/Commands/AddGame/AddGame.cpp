#include "AddGame.h"

extern std::vector<char*> messages;

AddGame::AddGame(GameManager* gameManager)
{
	this->callMessage = (char*)"AddGame";
	this->gameManager = gameManager;
}

void AddGame::OnCall(char* message)
{
	if (*(message + strlen(callMessage) + 1) == '\0')
	{
		//handle pop with with text inputs;
		Console::messages.push_back((char*)"Opening gui");
		openGUI = true;
	}
	else
	{
		char* param1 = strchr(message, ' ') + 1;
		if ((uintptr_t)param1 > 0x1000)
			*(param1 - 1) = '\0';
		else
		{
			Console::messages.push_back((char*)"1 parameter was unspecified");
			return;
		}

		char* param2 = strchr(param1, ' ') + 1;
		if ((uintptr_t)param2 > 0x1000)
			*(param2 - 1) = '\0';
		else
		{
			Console::messages.push_back((char*)"2 parameter was unspecified");
			return;
		}

		char* param3 = strchr(param2, ' ') + 1;
		if ((uintptr_t)param3 > 0x1000)
			*(param3 - 1) = '\0';
		else
		{
			Console::messages.push_back((char*)"3 parameter was unspecified");
			return;
		}

		char* param4 = strchr(param3, ' ') + 1;
		if ((uintptr_t)param4 > 0x1000)
			*(param4 - 1) = '\0';
		else
		{
			Console::messages.push_back((char*)"4 parameter was unspecified");
			return;
		}

		char* param5 = strchr(param4, ' ') + 1;
		if ((uintptr_t)param5 > 0x1000)
			*(param5 - 1) = '\0';
		else
		{
			Console::messages.push_back((char*)"5 parameter was unspecified");
			return;
		}

		char* param6 = strchr(param5, ' ') + 1;
		if ((uintptr_t)param6 > 0x1000)
			*(param6 - 1) = '\0';
		else
		{
			Console::messages.push_back((char*)"6 parameter was unspecified");
			return;
		}

		char* param7 = strchr(param6, ' ') + 1;
		if ((uintptr_t)param7 > 0x1000)
			*(param7 - 1) = '\0';
		else
		{
			Console::messages.push_back((char*)"7 parameter was unspecified");
			return;
		}

		char* param8 = strchr(param7, ' ') + 1;
		if ((uintptr_t)param8 > 0x1000)
			*(param8 - 1) = '\0';
		else
		{
			Console::messages.push_back((char*)"8 parameter was unspecified");
			return;
		}

		char* param9 = strchr(param8, ' ') + 1;
		if ((uintptr_t)param9 > 0x1000)
			*(param9 - 1) = '\0';
		else
		{
			gameManager->gameList.push_back(Game{ param1, param2, param3, param4, param5, param6, param7, param8 });
			Console::messages.push_back((char*)"Game was added with a dllPath");
			return;
		}

		gameManager->gameList.push_back(Game{ param1, param2, param3, param4, param5, param6, param7, param8, param9 });
		Console::messages.push_back((char*)"Game was added with a downloadURL and nameURL");
		return;
	}
}