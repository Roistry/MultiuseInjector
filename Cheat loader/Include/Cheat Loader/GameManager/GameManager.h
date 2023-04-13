#pragma once
#include <vector>

#include "Cheat Loader/Game/Game.h"

class GameManager
{
public:
    std::vector<Game> gameList;
    int selectedGame;

    GameManager();
    void DisplaySelectAllGames();
    void DisplayInfoSelectedGame();
    Game* GetSelectedGame();
};