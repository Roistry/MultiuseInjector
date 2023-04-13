#include "Game.h"

Game::Game(const char* gameName, const char* processName, const char* graphicsEngine, const char* gameEngine, const char* antiCheat, const char* obfuscator, const char* packer, const char* dllPath)
{
    this->gameName = gameName;
    this->processName = processName;

    this->graphicsEngine = graphicsEngine;
    this->gameEngine = gameEngine;
    this->antiCheat = antiCheat;
    this->obfuscator = obfuscator;
    this->packer = packer;

    this->dllPath = dllPath;

    this->downloadURL = nullptr;
    this->downloadName = nullptr;
}

Game::Game(const char* gameName, const char* processName, const char* graphicsEngine, const char* gameEngine, const char* antiCheat, const char* obfuscator, const char* packer, const char* downloadURL, const char* downloadName)
{
    this->gameName = gameName;
    this->processName = processName;

    this->graphicsEngine = graphicsEngine;
    this->gameEngine = gameEngine;
    this->antiCheat = antiCheat;
    this->obfuscator = obfuscator;
    this->packer = packer;

    this->dllPath = nullptr;

    this->downloadURL = downloadURL;
    this->downloadName = downloadName;
}

void Game::DisplayMenuText()
{
    ImGui::Text("Game Name: %s", gameName);
    ImGui::Text("Graphics Engine: %s", graphicsEngine);
    ImGui::Text("Game Engine: %s", gameEngine);
    ImGui::Text("AntiCheat: %s", antiCheat);
    ImGui::Text("Obfuscator: %s", obfuscator);
    ImGui::Text("Packer: %s", packer);
}

bool Game::Download()
{
    if (S_OK != URLDownloadToFileA(0, downloadURL, downloadName, 0, 0))
        return false;
    return true;
}