#include "Console.h"

void TextList(std::vector<char*> texts)
{
	for (size_t i = 0; i < texts.size(); i++)
		ImGui::Text(texts[i]);
}

void Console::AddCommand(CommandBase* commandBase)
{
	commands.push_back(commandBase);
}

void Console::SendMessageToConsole(char* message)
{
	char* newMessage = new char[260];
	strcpy_s(newMessage, 260, message);
	messages.push_back(newMessage);

	char* strCompare = strchr(message, ' ');
	if (strCompare)
		*strCompare = '\0';

	for (size_t i = 0; i < commands.size(); i++)
		if (strcmp(commands[i]->callMessage, message) == 0)
		{
			if (strCompare)
				*strCompare = ' ';
			this->commands[i]->OnCall(message);
		}
}

void Console::UpdateImGuiConsole()
{
	TextList(messages);
}