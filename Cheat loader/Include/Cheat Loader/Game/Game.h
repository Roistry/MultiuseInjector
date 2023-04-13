#pragma once
#include <string>

#include <urlmon.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx9.h"

#pragma comment(lib, "Urlmon.lib")

#define Opengl                  "Opengl"                    //Graphic Engine
#define D3d9                    "D3d9"                      //Graphic Engine
#define D3d10                   "D3d10"                     //Graphic Engine
#define D3d11                   "D3d11"                     //Graphic Engine
#define Vulkan                  "Vulkan"                    //Graphic Engine

#define Cube                    "Cube"                      //Game Engine
#define SourceEngine            "Source Engine"             //Game Engine
#define SourceEngine2           "Source Engine 2"           //Game Engine
#define UnrealEngine2           "Unreal Engine 2"           //Game Engine
#define UnrealEngine3           "Unreal Engine 3"           //Game Engine
#define UnrealEngine4           "Unreal Engine 4"           //Game Engine
#define UnrealEngine5           "Unreal Engine 5"           //Game Engine
#define UnityIl2cpp             "Unity Il2cpp"              //Game Engine
#define UnityMono               "Unity Mono"                //Game Engine

#define EasyAntiCheat           "EasyAntiCheat"             //Anticheat
#define PunkBuster              "PunkBuster"                //Anticheat
#define BattleEye               "BattleEye"                 //Anticheat
#define Valve                   "Valve"                     //Anticheat
#define FairFlight              "FairFlight"                //Anticheat
#define TencentAntiCheatExpert  "Tencent AntiCheatExpert"   //Anticheat
#define GameGaurd               "GameGaurd"                 //Anticheat
#define XIGNCODE3               "XIGNCODE3"                 //Anticheat
#define Ricochet                "Ricochet"                  //Anticheat
#define HackShield              "HackShield"                //Anticheat
#define Anybrain                "Anybrain"                  //Anticheat
#define EQU8                    "EQU8"                      //Anticheat
#define NexonGameSecurity       "Nexon GameSecurity"        //Anticheat
#define CodeStage               "Code Stage"                //Anticheat

#define Beebyte                 "Beebyte"                   //Obfuscator

#define Themida                 "Themida"                   //Packer

#define None                    "None"                      //None
#define Unknown                 "Unknown"                   //Unknown

class Game
{
public:
    const char* gameName;
    const char* processName;

    const char* graphicsEngine;
    const char* gameEngine;
    const char* antiCheat;
    const char* obfuscator;
    const char* packer;

    const char* dllPath;

    const char* downloadURL;
    const char* downloadName;

    Game(const char* gameName, const char* processName, const char* graphicsEngine, const char* gameEngine, const char* antiCheat, const char* obfuscator, const char* packer, const char* dllPath);
    Game(const char* gameName, const char* processName, const char* graphicsEngine, const char* gameEngine, const char* antiCheat, const char* obfuscator, const char* packer, const char* downloadURL, const char* downloadName);

    void DisplayMenuText();
    bool Download();
};