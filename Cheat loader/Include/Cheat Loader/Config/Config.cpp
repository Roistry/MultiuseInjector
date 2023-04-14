#include "Config.h"

using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

Config::Config(ConfigParams* _ConfigParams)
{
	this->m_pConfigParams = _ConfigParams;
}

void Config::SaveConfig()
{
	ordered_json c_Json;
    c_Json["ProcessList"]["Search"]["Save"] = *this->m_pConfigParams->m_bSaveSearch;
    c_Json["ProcessList"]["Search"]["Info"] = this->m_pConfigParams->m_strSearch;
    c_Json["ProcessList"]["Filters"]["Save"] = *this->m_pConfigParams->m_bSaveFilters;
    c_Json["ProcessList"]["Sorts"]["Save"] = *this->m_pConfigParams->m_bSaveSorts;
    c_Json["ProcessList"]["AutoRefresh"]["Save"] = *this->m_pConfigParams->m_bSaveAutoRefresh;
    c_Json["ProcessList"]["AutoRefresh"]["Info"] = *this->m_pConfigParams->m_bAutoRefresh;

    c_Json["Inject"]["ProcessName"]["Save"] = *this->m_pConfigParams->m_bSaveProcessName;
    c_Json["Inject"]["ProcessName"]["Info"] = this->m_pConfigParams->m_strProcessName;
    c_Json["Inject"]["DllPath"]["Save"] = *this->m_pConfigParams->m_bSaveDllPath;
    c_Json["Inject"]["DllPath"]["Info"] = this->m_pConfigParams->m_strDllPath;
    c_Json["Inject"]["InjectionMethod"]["Save"] = *this->m_pConfigParams->m_bSaveInjectionMethod;
    c_Json["Inject"]["InjectionMethod"]["Info"] = *this->m_pConfigParams->m_iInjectionMethod;
    c_Json["Inject"]["LaunchMethod"]["Save"] = *this->m_pConfigParams->m_bSaveLaunchMethod;
    c_Json["Inject"]["LaunchMethod"]["Info"] = *this->m_pConfigParams->m_iLaunchMethod;

    c_Json["GameList"]["Games"]["Save"] = *this->m_pConfigParams->m_bSaveGames;

	std::ofstream c_Stream;
    c_Stream.open("Config.json");
    c_Stream << std::setw(2) << c_Json;
    c_Stream.close();
}

void Config::LoadConfig()
{
    FILE* f;
    fopen_s(&f, "Config.json", "r");
    if (f)
        fclose(f);
    else
        return;

    std::ifstream c_Stream;
    c_Stream.open("Config.json");
    if (c_Stream.bad())
        return;

    json json = json::parse(c_Stream);
    c_Stream.close();

    *this->m_pConfigParams->m_bSaveSearch = json["ProcessList"]["Search"].value("Save", false);
    if (*this->m_pConfigParams->m_bSaveSearch)
        memcpy(this->m_pConfigParams->m_strSearch, (char*)(json["ProcessList"]["Search"].value("Info", "\0").c_str()), strlen((char*)(json["ProcessList"]["Search"].value("Info", "\0").c_str())));
    *this->m_pConfigParams->m_bSaveFilters = json["ProcessList"]["Filters"].value("Save", false);
    *this->m_pConfigParams->m_bSaveSorts = json["ProcessList"]["Sorts"].value("Save", false);
    *this->m_pConfigParams->m_bSaveAutoRefresh = json["ProcessList"]["AutoRefresh"].value("Save", false);
    if (*this->m_pConfigParams->m_bSaveAutoRefresh)
        *this->m_pConfigParams->m_bAutoRefresh = json["ProcessList"]["AutoRefresh"].value("Info", false);

    *this->m_pConfigParams->m_bSaveProcessName = json["Inject"]["ProcessName"].value("Save", false);
    if (*this->m_pConfigParams->m_bSaveProcessName)
        memcpy(this->m_pConfigParams->m_strProcessName, (char*)(json["Inject"]["ProcessName"].value("Info", "\0").c_str()), strlen((char*)(json["Inject"]["ProcessName"].value("Info", "\0").c_str())));
    *this->m_pConfigParams->m_bSaveDllPath = json["Inject"]["DllPath"].value("Save", false);
    if (*this->m_pConfigParams->m_bSaveDllPath)
        memcpy(this->m_pConfigParams->m_strDllPath, (char*)(json["Inject"]["DllPath"].value("Info", "\0").c_str()), strlen((char*)(json["Inject"]["DllPath"].value("Info", "\0").c_str())));
    *this->m_pConfigParams->m_bSaveInjectionMethod = json["Inject"]["InjectionMethod"].value("Save", false);
    if (*this->m_pConfigParams->m_bSaveInjectionMethod)
        *this->m_pConfigParams->m_iInjectionMethod = json["Inject"]["InjectionMethod"].value("Info", 0);
    *this->m_pConfigParams->m_bSaveLaunchMethod = json["Inject"]["LaunchMethod"].value("Save", false);
    if (*this->m_pConfigParams->m_bSaveLaunchMethod)
        *this->m_pConfigParams->m_iLaunchMethod = json["Inject"]["LaunchMethod"].value("Info", 0);

    *this->m_pConfigParams->m_bSaveGames = json["GameList"]["Games"].value("Save", false);
}

void Config::DeleteConfig()
{
    remove("Config.json");
}