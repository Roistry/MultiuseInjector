#pragma once
#include <fstream>

#include "Json/json.hpp"
#include "Cheat Loader/Game/GameManager/GameManager.h"

struct ConfigParams
{
    bool* m_bSaveSearch; char* m_strSearch = new char[MAX_PATH];
    bool* m_bSaveFilters;
    bool* m_bSaveSorts;
    bool* m_bSaveAutoRefresh; bool* m_bAutoRefresh;

    bool* m_bSaveProcessName; char* m_strProcessName = new char[MAX_PATH];
    bool* m_bSaveDllPath; char* m_strDllPath = new char[MAX_PATH];
    bool* m_bSaveInjectionMethod; int* m_iInjectionMethod;
    bool* m_bSaveLaunchMethod; int* m_iLaunchMethod;

    bool* m_bSaveGames; GameManager** m_pGameManager;
};

class Config
{
public:
    ConfigParams* m_pConfigParams;

	Config(ConfigParams* _ConfigParams);

	void SaveConfig();
    void LoadConfig();
    void DeleteConfig();
};