#pragma once
#include <vector>

#include "ImGui/imgui.h"
#include "Cheat Loader/Console/CommandBase/CommandBase.h"

class Console
{
public:
	static std::vector<char*>messages;
	std::vector<CommandBase*> commands;

	void AddCommand(CommandBase* commandBase);
	void SendMessageToConsole(char* message);
	void UpdateImGuiConsole();
};