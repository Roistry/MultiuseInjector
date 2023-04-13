#include "GameManager.h"

GameManager::GameManager()
{
    selectedGame = 0;
}

void GameManager::DisplaySelectAllGames()
{
	for (size_t i = 0; i < gameList.size(); i++)
	{
        ImGui::SetCursorPos(ImVec2(0, i * 20));
		ImGui::PushID(i);
		if (ImGui::HighlightedButton(gameList[i].gameName, ImVec2{ 240, 20 }, i == selectedGame, ImVec2(0, i * 20), ImVec2{ 240, 20 }, ImVec4{ 255 / 255, 255 / 255, 255 / 255, 0.3f }))
			selectedGame = i;
		ImGui::PopID();
	}
}

void GameManager::DisplayInfoSelectedGame()
{
	gameList[selectedGame].DisplayMenuText();
}

Game* GameManager::GetSelectedGame()
{
	return &gameList[selectedGame];
}
