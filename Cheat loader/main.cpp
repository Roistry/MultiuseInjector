#include <iostream>
#include <string>
#include <vector>
#include <chrono>

#include <Windows.h>
#include <TlHelp32.h>

#include "Window/ImGuiD3d9Window/ImGuiD3d9Window.h"
#include "Cheat Loader/Game/GameManager/GameManager.h"
#include "Cheat Loader/Console/Console.h"
#include "Cheat Loader/Console/Commands/Clear/Clear.h"
#include "Cheat Loader/Console/Commands/Exit/Exit.h"
#include "Cheat Loader/InjectorUtils/InjectorUtils.h"
#include "ImGui/imimgui.h"
#include "Cheat Loader/PatchNotesDate/PatchNotesDate.h"
#include "Cheat Loader/PatchNotesDate/PatchNotesDateManager/PatchNotesDateManager.h"
#include "Cheat Loader/Console/Commands/AddGame/AddGame.h"
#include "Cheat Loader/Config/Config.h"

bool shouldExit = false;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wideparam, LPARAM lowparam);
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wideparam, LPARAM lowparam)
{
    ImGui_ImplWin32_WndProcHandler(hwnd, msg, wideparam, lowparam);
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        shouldExit = true;
        break;
    }

    return DefWindowProcA(hwnd, msg, wideparam, lowparam);
}

namespace menu
{
    ImVec2 windowResolution{ 850, 250 };

    int selectedCategory = 0;

    //ProcessList
    bool bSearch;
    bool bFilters;
    bool bSorts;
    bool bAutoRefresh;

    //Inject
    bool bProcessName;
    bool bDllPath;
    bool bInjectionMethod;
    bool bLaunchMethod;

    //GameList
    bool bGames;

    //int selectedGenre = 0;
    //const char* gameGenres[] = { "Genre: ALL", "Genre: ONLINE", "Genre: MMO", "Genre: SINGLE PLAYER"};
    //bool bGameGenresSelectTab = false;
}

bool smoothCheckboxes = false;
int smoothCheckboxes1 = 0;
void SmoothButtons(int amount, ImVec2 buttonSize)
{
    static std::chrono::steady_clock::time_point currentTime;
    static std::chrono::steady_clock::time_point lastTime;

    currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);

    if (smoothCheckboxes)
    {
        if (timePassed.count() > 10.0f)
        {
            lastTime = std::chrono::high_resolution_clock::now();
            if (smoothCheckboxes1 != amount)
                smoothCheckboxes1++;
        }
        for (size_t i = 0; i < smoothCheckboxes1; i++)
        {
            ImGui::Button("Lmao", buttonSize);
        }
    }
    else
        smoothCheckboxes1 = 0;
}

DWORD Window(ImGuiD3d9Window* mainWindow)
{
    GameManager gameManager;

    Console console;
    Clear* clear = new Clear{};
    console.AddCommand(clear);
    Exit* exit = new Exit{};
    console.AddCommand(exit);
    AddGame* addGame = new AddGame(&gameManager);
    console.AddCommand(addGame);

    PatchNotesDateManager patchNotesDateManager;
    patchNotesDateManager.vecPatchNotesDates.push_back(PatchNotesDate{ "3/28/2023", "https://cdn.discordapp.com/attachments/765166083401187338/1090396595499044944/3-28-2023_Cheat_loader.exe", "1. Process list showing all processes\nthat has a box to search processes,\na checkbox to auto refresh/not to and a\nbutton that if you dont want to auto\nrefresh you can manually do it.\n\n2. Inject tab showing different\ninjection types, box to type process\nname and if valid shows pid and\narchitecture box for dll path and a box\nto fill whats explained next.\n\n3. Game List tab where you you have\npresets of games and then auto fill\nfield in inject depends on what game\nyou selected.\n\n4. Finally an about tab showing basic\nstuff about me and a small patch notes\ntab I will change later.\n\n" });
    patchNotesDateManager.vecPatchNotesDates.push_back(PatchNotesDate{ "3/29/2023", "https://cdn.discordapp.com/attachments/765166083401187338/1090747709461364826/3-29-2023_Cheat_loader.exe", "1. Added this thing where you can\ndownload previous versions of the\nloader with a selectable date and a\ndownload button.\n\n2. Added command called AddGame where\nyou can add games to the game list with\na popup where you input information\nabout the game, you can leave it blank\nand it will fill as None.\n\n3. Finally added inject button in the\ninject tab to inject to whatever\nprocess by specifying a process/pid and\na dll path, currently only LoadLibraryA\ninject method works and in the launch\nmethods only NtCreateThreadEx and\nHijackThread works.\n\n" });
    patchNotesDateManager.vecPatchNotesDates.push_back(PatchNotesDate{ "4/13/2023", " ", "1. Removed default games in game list tab\n\n2. Removed Del key intentially closing the loader.\n\n3. Program wont crash if there is 0 games in game list tab, did the same in game list tab.\n\n4. Didnt realise last build that the injecting will only work on x64 so i made if x64 and x86 compatible.\n\n5. If you left Game Engine in AddGame blank it will stay blank so i fixed it to be like the others and it will be None instead.\n\n6. Added a config system that you can controll in the Settings tab, there you can choose what to load upon opening the injector.\n\n" });

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();

    io.IniFilename = 0;
    ImFont* fontConsola = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Consola.ttf", 16.0f);
    ImFont* fontConsolaSmall = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Consola.ttf", 12.0f);

    bool autoRefresh = false;
    char searchFilter[MAX_PATH]{ 0 };

    char process[MAX_PATH] = { 0 };
    char dllpath[MAX_PATH] = { 0 };
    int pid = 0;
    int architecture = 0;
    int injectMethodsSelected = 0;
    int launchMethodsSelected = 0;

    ConfigParams configParams{ 0 };
    {
        configParams.m_bSaveSearch = &menu::bSearch;
        configParams.m_bSaveFilters = &menu::bFilters;
        configParams.m_bSaveSorts = &menu::bSorts;
        configParams.m_bSaveAutoRefresh = &menu::bAutoRefresh;

        configParams.m_bSaveProcessName = &menu::bProcessName;
        configParams.m_bSaveDllPath = &menu::bDllPath;
        configParams.m_bSaveInjectionMethod = &menu::bInjectionMethod;
        configParams.m_bSaveLaunchMethod = &menu::bLaunchMethod;

        configParams.m_bSaveGames = &menu::bGames;

        configParams.m_bAutoRefresh = &autoRefresh;
        configParams.m_strSearch = searchFilter;

        configParams.m_strProcessName = process;
        configParams.m_strDllPath = dllpath;
        configParams.m_iInjectionMethod = &injectMethodsSelected;
        configParams.m_iLaunchMethod = &launchMethodsSelected;
    }
    Config config{ &configParams };
    config.LoadConfig();
    while (!shouldExit)
    {
        ImGui_ImplWin32_NewFrame();
        ImGui_ImplDX9_NewFrame();
        ImGui::NewFrame();
        //Start Draw
        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowSize(ImVec2(menu::windowResolution.x, menu::windowResolution.y));

        style.WindowPadding = ImVec2(0, 0);
        style.FramePadding = ImVec2(0, 0);

        ImGui::Begin("NAME", (bool*)true, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDecoration);
        {
            ImGui::PushFont(fontConsola);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(29.0f / 255, 29.0f / 255, 29.0f / 255, 1));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(49.0f / 255, 50.0f / 255, 49.0f / 255, 1));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(49.0f / 255, 50.0f / 255, 49.0f / 255, 1));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(59.0f / 255, 60.0f / 255, 59.0f / 255, 1));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(49.0f / 255, 50.0f / 255, 49.0f / 255, 1));

            ImGui::SetCursorPos(ImVec2(0, 0));
            ImGui::BeginChild("Info", ImVec2(240, 30), false);
            {
                ImGui::Text("Cheat loader");
                ImGui::EndChild();
            }

            ImGui::SetCursorPos(ImVec2(0, 30));
            ImGui::BeginChild("Console", ImVec2(240, 190), true);
            {
                static char message[MAX_PATH];

                ImGui::PushFont(fontConsolaSmall);
                console.UpdateImGuiConsole();
                ImGui::PopFont();

                ImGui::SetCursorPos(ImVec2(0, 165));
                if (ImGui::InputText(" ", message, MAX_PATH, ImVec2{ ImGui::GetWindowWidth(), 0 }, ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    console.SendMessageToConsole(message);
                    ZeroMemory(message, strlen(message));
                }
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Commands:");
                    for (size_t i = 0; i < console.commands.size(); i++)
                        ImGui::Text(console.commands[i]->callMessage);
                    ImGui::EndTooltip();
                }
                ImGui::EndChild();
            }

            ImGui::SetCursorPos(ImVec2(240, 0));
            ImGui::BeginChild("Process List", ImVec2(120, 30), false);
            {
                if (ImGui::HighlightedButton("Process List", ImVec2{ 120, 30 }, menu::selectedCategory == 0, ImVec2{ 0, 0 }, ImVec2{ 120, 30 }, ImVec4{ 255 / 255, 255 / 255, 255 / 255, 0.3f }))
                    menu::selectedCategory = 0;
                ImGui::EndChild();
            }

            ImGui::SetCursorPos(ImVec2(360, 0));
            ImGui::BeginChild("Inject", ImVec2(120, 30), false);
            {
                if (ImGui::HighlightedButton("Inject", ImVec2{ 120, 30 }, menu::selectedCategory == 1, ImVec2{ 0, 0 }, ImVec2{ 120, 30 }, ImVec4{ 255 / 255, 255 / 255, 255 / 255, 0.3f }))
                    menu::selectedCategory = 1;
                ImGui::EndChild();
            }

            ImGui::SetCursorPos(ImVec2(480, 0));
            ImGui::BeginChild("Game List", ImVec2(120, 30), false);
            {
                if (ImGui::HighlightedButton("Game List", ImVec2{ 120, 30 }, menu::selectedCategory == 2, ImVec2{ 0, 0 }, ImVec2{ 120, 30 }, ImVec4{ 255 / 255, 255 / 255, 255 / 255, 0.3f }))
                    menu::selectedCategory = 2;
                ImGui::EndChild();
            }

            ImGui::SetCursorPos(ImVec2(600, 0));
            ImGui::BeginChild("About", ImVec2(120, 30), false);
            {
                if (ImGui::HighlightedButton("About", ImVec2{ 120, 30 }, menu::selectedCategory == 3, ImVec2{ 0, 0 }, ImVec2{ 120, 30 }, ImVec4{ 255 / 255, 255 / 255, 255 / 255, 0.3f }))
                    menu::selectedCategory = 3;
                ImGui::EndChild();
            }

            ImGui::SetCursorPos(ImVec2(720, 0));
            ImGui::BeginChild("Settings", ImVec2(120, 30), false);
            {
                if (ImGui::HighlightedButton("Settings", ImVec2{ 120, 30 }, menu::selectedCategory == 4, ImVec2{ 0, 0 }, ImVec2{ 120, 30 }, ImVec4{ 255 / 255, 255 / 255, 255 / 255, 0.3f }))
                    menu::selectedCategory = 4;
                ImGui::EndChild();
            }

            ImGui::SetCursorPos(ImVec2(240, 30));
            ImGui::BeginChild("Category", ImVec2(ImGui::GetWindowWidth() - 240, 190), false);
            {
                //Process List
                if (menu::selectedCategory == 0)
                {
                    static std::vector<std::pair<char*, DWORD>> processes;

                    ImGui::SetCursorPos(ImVec2{ 120 * 4, 0 });
                    ImGui::PushID(0);
                    ImGui::Checkbox(" ", &autoRefresh);
                    ImGui::PopID();
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::Text("Auto Refresh");
                        ImGui::EndTooltip();
                    }

                    if (!autoRefresh)
                    {
                        ImGui::SetCursorPos(ImVec2{ 120 * 4 + 16, 0 });
                        if (ImGui::Button("  Refresh  "))
                        {
                            for (size_t i = 0; i < processes.size(); i++)
                                delete[] processes[i].first;

                            HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

                            PROCESSENTRY32 pe32;
                            pe32.dwSize = sizeof(pe32);

                            size_t processAmount = 0;
                            Process32First(hSnapshot, &pe32);
                            do
                            {
                                processes.resize(++processAmount);
                                size_t processNameLen = strlen(pe32.szExeFile) + 1;
                                processes[processAmount - 1].first = new char[processNameLen];

                                strcpy_s(processes[processAmount - 1].first, processNameLen, pe32.szExeFile);
                                processes[processAmount - 1].second = pe32.th32ProcessID;
                            } while (Process32Next(hSnapshot, &pe32));
                            CloseHandle(hSnapshot);
                        }
                    }
                    else
                    {
                        for (size_t i = 0; i < processes.size(); i++)
                            delete[] processes[i].first;

                        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

                        PROCESSENTRY32 pe32;
                        pe32.dwSize = sizeof(pe32);

                        size_t processAmount = 0;
                        Process32First(hSnapshot, &pe32);
                        do
                        {
                            processes.resize(++processAmount);
                            size_t processNameLen = strlen(pe32.szExeFile) + 1;
                            processes[processAmount - 1].first = new char[processNameLen];

                            strcpy_s(processes[processAmount - 1].first, processNameLen, pe32.szExeFile);
                            processes[processAmount - 1].second = pe32.th32ProcessID;
                        } while (Process32Next(hSnapshot, &pe32));
                        CloseHandle(hSnapshot);
                    }

                    ImGui::SetCursorPos(ImVec2{ 0, 0 });
                    if (ImGui::InputText(" ", searchFilter, MAX_PATH, ImVec2{ 120 * 2, 0 }))
                    {
                        
                    }

                    ImGui::SetCursorPos(ImVec2{ 120 * 2, 0 });
                    ImGui::Text("Filters");

                    ImGui::SetCursorPos(ImVec2{ 120 * 3, 0 });
                    ImGui::Text("Sort");

                    float nextProcessInfoOffset = 0.0f;
                    for (size_t i = 0; i < processes.size(); i++)
                    {
                        bool validFiltered = true;
                        for (size_t j = 0; j < strlen(searchFilter); j++)
                        {
                            if (processes[i].first[j] != searchFilter[j])
                            {
                                validFiltered = false;
                                break;
                            }
                        }
                        if (validFiltered)
                        {
                            ImGui::SetCursorPos(ImVec2{ 0, 30 + nextProcessInfoOffset * 20 });
                            DWORD architecture = IsWow64(processes[i].second);
                            if (!architecture)
                                ImGui::Text("(?)");
                            else
                                ImGui::Text("(%s)", architecture == x64Process ? "x64" : "x86");

                            ImGui::SetCursorPos(ImVec2{ 50, 30 + nextProcessInfoOffset * 20 });
                            ImGui::Text("Process: %s", processes[i].first);

                            ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowWidth() - 120, 30 + nextProcessInfoOffset * 20 });
                            ImGui::Text("PID: %u", processes[i].second);
                            nextProcessInfoOffset++;
                        }

                    }
                    ImGui::NewLine();
                }

                //Inject
                if (menu::selectedCategory == 1)
                {
                    ImGui::Text("Process:");
                    ImGui::SetCursorPos(ImVec2(120, 0));
                    ImGui::PushID(0);
                    if (ImGui::InputText(" ", process, MAX_PATH))
                    {
                        
                    }
                    ImGui::PopID();
                    pid = GetPID(process);

                    ImGui::Text("PID:");
                    ImGui::SetCursorPos(ImVec2(120, 20));
                    ImGui::PushID(1);
                    if (ImGui::InputInt(" ", &pid, 4))
                    {
                        char* processName = GetProcessName(pid);
                        if (processName)
                            strcpy_s(process, processName);
                        else
                            ZeroMemory(process, strlen(process));
                    }
                    ImGui::PopID();

                    ImGui::SetCursorPos(ImVec2(240, 20));
                    ImGui::Text("Architecture:");
                    ImGui::SetCursorPos(ImVec2(360, 20));
                    architecture = IsWow64(pid);
                    if (!architecture)
                        ImGui::Text("(?)");
                    else
                        ImGui::Text("(%s)", architecture == x64Process ? "x64" : "x86");

                    ImGui::NewLine();

                    ImGui::Text("Dll Path:");
                    ImGui::SetCursorPos(ImVec2(120, 60));
                    ImGui::PushID(2);
                    if (ImGui::InputText(" ", dllpath, MAX_PATH))
                    {
                        memcpy(configParams.m_strDllPath, dllpath, strlen(dllpath));
                    }
                    ImGui::PopID();

                    static const char* injectMethods[] = { "LoadLibrayA", "ManualMap" };
                    static bool injectMethodsListOpen = false;
                    ImGui::SelectablesList(injectMethods, 2, ImVec2{ 0, 80 }, ImVec2{ 200, 15 }, &injectMethodsSelected, &injectMethodsListOpen);
                    configParams.m_iInjectionMethod = &injectMethodsSelected;

                    static const char* launchMethods[] = { "NtCreateThreadEx", "Thread Hijacking", "SetWindowHookEx", "QueueUserAPC" };
                    static bool launchMethodsListOpen = false;
                    ImGui::SelectablesList(launchMethods, 4, ImVec2{ 240, 80 }, ImVec2{ 200, 15 }, &launchMethodsSelected, &launchMethodsListOpen);
                    configParams.m_iLaunchMethod = &launchMethodsSelected;

                    if (!injectMethodsListOpen && !launchMethodsListOpen)
                    {
                        ImGui::NewLine();

                        ImGui::SetCursorPos(ImVec2(130, 100));
                        if (ImGui::Button("Inject", ImVec2{ 180, 15 }))
                            Inject(process, dllpath, (LaunchMethod)launchMethodsSelected, (InjectMethod)injectMethodsSelected, architecture);

                        ImGui::NewLine();

                        static bool postConfirmed = false;
                        if (!postConfirmed)
                        {
                            if (ImGui::Button("Fill Fields From Selected Game?"))
                                postConfirmed = true;
                            ImGui::SameLine();
                            if (gameManager.gameList.size())
                                ImGui::Text("(%s)", gameManager.gameList[gameManager.selectedGame].gameName);
                        }
                        else
                        {
                            ImGui::Text("Confirm? ");
                            if (ImGui::Button("No"))
                                postConfirmed = false;
                            ImGui::SameLine();
                            if (ImGui::Button("Yes"))
                            {
                                strcpy_s(process, gameManager.gameList[gameManager.selectedGame].processName);
                                if (gameManager.gameList[gameManager.selectedGame].dllPath)
                                    strcpy_s(dllpath, gameManager.gameList[gameManager.selectedGame].dllPath);
                                else
                                    strcpy_s(dllpath, gameManager.gameList[gameManager.selectedGame].downloadName);
                                postConfirmed = false;
                            }
                        }
                    }
                }

                //Game List
                if (menu::selectedCategory == 2)
                {
                    //games sorted to categories like mmo, online etc... not needed atm save for later
                    #if 0
                    if (ImGui::Button(menu::gameGenres[menu::selectedGenre], ImVec2{ 195, 20 }))
                        menu::bGameGenresSelectTab = !menu::bGameGenresSelectTab;
                    if (menu::bGameGenresSelectTab)
                    {
                        ImGui::BeginChild("Game Genres Select Tab");
                        {
                            for (size_t i = 0; i < ARRAYSIZE(menu::gameGenres); i++)
                            {
                                ImGui::SetCursorPos(ImVec2(0, i * 20));
                                if (ImGui::Button(menu::gameGenres[i], ImVec2{ 195, 20 }))
                                {
                                    menu::selectedGenre = i;
                                    menu::bGameGenresSelectTab = false;
                                }
                            }
                            ImGui::EndChild();
                        }
                    }
                    #endif

                    ImGui::SetCursorPos(ImVec2(0, 0));
                    gameManager.DisplaySelectAllGames();

                    ImGui::SetCursorPos(ImVec2(240, 0));
                    ImGui::BeginChild("Game Info", ImVec2{ ImGui::GetWindowWidth() / 2, ImGui::GetWindowHeight() }, false);
                    {
                        if (gameManager.gameList.size())
                        {
                            gameManager.DisplayInfoSelectedGame();
                            ImGui::SetCursorPos(ImVec2(0, 160));
                            if (gameManager.gameList[gameManager.selectedGame].downloadURL)
                                if (ImGui::Button("Download", ImVec2{ ImGui::GetWindowWidth(), 20.0 }))
                                    if (gameManager.gameList[gameManager.selectedGame].Download())
                                    {
                                        char* message = combineChars((char*)"Downloaded ", (char*)gameManager.gameList[gameManager.selectedGame].gameName);
                                        console.SendMessageToConsole(message);
                                        delete[] message;
                                    }
                                    else
                                    {
                                        char* message = combineChars((char*)"Coudlnt download ", (char*)gameManager.gameList[gameManager.selectedGame].gameName);
                                        console.SendMessageToConsole(message);
                                        delete[] message;
                                    }
                        }
                        ImGui::EndChild();
                    }
                }

                //About
                if (menu::selectedCategory == 3)
                {
                    ImGui::Text("Youtube: %s", "None");
                    ImGui::Text("Github: %s", "Roistry");
                    ImGui::Text("Discord: %s", "Roistry#4889");
                    ImGui::Text("Discord Server: %s", "None");

                    ImGui::SetCursorPos(ImVec2(240, 0));
                    ImGui::BeginChild("Patch Notes", ImVec2(360, 190), true);
                    {
                        ImGui::Text("Patch Notes ");
                        patchNotesDateManager.DisplaySelectAllDates();
                        ImGui::NewLine();
                        patchNotesDateManager.DisplayPatchNoteSelectedDate();

                        ImGui::SetCursorPos(ImVec2(240, 0));
                        if (ImGui::Button("Download", ImVec2{ 120, 20 }))
                            patchNotesDateManager.DownloadSelectedDate();
                        ImGui::EndChild();
                    }
                }

                //Settings
                if (menu::selectedCategory == 4)
                {
                    if (ImGui::CollapsingHeader("Load"))
                    {
                        ImGui::Text("   ");
                        ImGui::SameLine();
                        if (ImGui::CollapsingHeader("ProcessList"))
                        {
                            ImGui::Text("   ");
                            ImGui::SameLine();
                            ImGui::Checkbox("Search", &menu::bSearch);
                            ImGui::Text("   ");
                            ImGui::SameLine();
                            ImGui::Checkbox("Filters", &menu::bFilters);
                            ImGui::Text("   ");
                            ImGui::SameLine();
                            ImGui::Checkbox("Sorts", &menu::bSorts);
                            ImGui::Text("   ");
                            ImGui::SameLine();
                            ImGui::Checkbox("AutoRefresh", &menu::bAutoRefresh);
                        }
                        ImGui::Text("   ");
                        ImGui::SameLine();
                        if (ImGui::CollapsingHeader("Inject"))
                        {
                            ImGui::Text("   ");
                            ImGui::SameLine();
                            ImGui::Checkbox("ProcessName", &menu::bProcessName);
                            ImGui::Text("   ");
                            ImGui::SameLine();
                            ImGui::Checkbox("DllPath", &menu::bDllPath);
                            ImGui::Text("   ");
                            ImGui::SameLine();
                            ImGui::Checkbox("InjectionMethod", &menu::bInjectionMethod);
                            ImGui::Text("   ");
                            ImGui::SameLine();
                            ImGui::Checkbox("LaunchMethod", &menu::bLaunchMethod);
                        }
                        ImGui::Text("   ");
                        ImGui::SameLine();
                        if (ImGui::CollapsingHeader("GameList"))
                        {
                            ImGui::Text("   ");
                            ImGui::SameLine();
                            ImGui::Checkbox("Games (Not working yet)", &menu::bGames);
                        }
                        ImGui::NewLine();
                    }
                }
                ImGui::EndChild();
            }

            if (addGame->openGUI)
            {
                int rows = 5;
                int columns = 2;

                float rowWidth = 100;
                float columnHeight = 100;

                ImVec2 test = ImVec2{ ImGui::GetWindowWidth() / 2 - ((rows * rowWidth) / 2), ImGui::GetWindowHeight() / 2 - ((columns * columnHeight) / 2) - 17.5f };
                ImGui::SetCursorPos(test);
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(100.0f / 255, 100.0f / 255, 100.0f / 255, 1));
                ImGui::BeginChild("WORKK", ImVec2{ rows * rowWidth, 200 }, false);
                {
                    static char* gameName = new char[MAX_PATH] { 0 };
                    static char* processName = new char[MAX_PATH] { 0 };
                    static char* graphicsEngine = new char[MAX_PATH] { 0 };
                    static char* gameEngine = new char[MAX_PATH] { 0 };
                    static char* antiCheat = new char[MAX_PATH] { 0 };
                    static char* obfuscator = new char[MAX_PATH] { 0 };
                    static char* packer = new char[MAX_PATH] { 0 };
                    static char* dllPath = new char[MAX_PATH] { 0 };
                    static char* downloadURL = new char[MAX_PATH] { 0 };
                    static char* downloadName = new char[MAX_PATH] { 0 };

                    ImGui::SetCursorPos(ImVec2{ 15, 15 });
                    ImGui::Text("Game Name");
                    ImGui::SameLine();
                    ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowWidth() / 2 - 95, 15 });
                    ImGui::PushID(0);
                    ImGui::InputText(" ", gameName, MAX_PATH, ImVec2{ 100, 15 });
                    ImGui::PopID();

                    ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowWidth() / 2 + 15, 15 });
                    ImGui::Text("Process Name");
                    ImGui::SameLine();
                    ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowWidth() - 115, 15 });
                    ImGui::PushID(1);
                    ImGui::InputText(" ", processName, MAX_PATH, ImVec2{ 100, 15 });
                    ImGui::PopID();

                    ImGui::SetCursorPos(ImVec2{ 15, 45 });
                    ImGui::Text("Graphics Engine");
                    ImGui::SameLine();
                    ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowWidth() / 2 - 95, 45 });
                    ImGui::PushID(2);
                    ImGui::InputText(" ", graphicsEngine, MAX_PATH, ImVec2{ 100, 15 });
                    ImGui::PopID();

                    ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowWidth() / 2 + 15, 45 });
                    ImGui::Text("Game Engine");
                    ImGui::SameLine();
                    ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowWidth() - 115, 45 });
                    ImGui::PushID(3);
                    ImGui::InputText(" ", gameEngine, MAX_PATH, ImVec2{ 100, 15 });
                    ImGui::PopID();

                    ImGui::SetCursorPos(ImVec2{ 15, 75 });
                    ImGui::Text("Anticheat");
                    ImGui::SameLine();
                    ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowWidth() / 2 - 95, 75 });
                    ImGui::PushID(4);
                    ImGui::InputText(" ", antiCheat, MAX_PATH, ImVec2{ 100, 15 });
                    ImGui::PopID();

                    ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowWidth() / 2 + 15, 75 });
                    ImGui::Text("Obufscator");
                    ImGui::SameLine();
                    ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowWidth() - 115, 75 });
                    ImGui::PushID(5);
                    ImGui::InputText(" ", obfuscator, MAX_PATH, ImVec2{ 100, 15 });
                    ImGui::PopID();

                    ImGui::SetCursorPos(ImVec2{ 15, 105 });
                    ImGui::Text("Packer");
                    ImGui::SameLine();
                    ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowWidth() / 2 - 95, 105 });
                    ImGui::PushID(6);
                    ImGui::InputText(" ", packer, MAX_PATH, ImVec2{ 100, 15 });
                    ImGui::PopID();

                    static bool dllPathGame = true;
                    ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowWidth() / 2 + 15, 105 });
                    if (ImGui::HighlightedButton("DllPath", ImVec2{ 100, 15 }, dllPathGame, ImVec2{ ImGui::GetWindowWidth() / 2 + 19, 107 }, ImVec2{ 96, 13 }, ImVec4{ 255 / 255, 255 / 255, 255 / 255, 0.3f }))
                        dllPathGame = true;

                    ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowWidth() / 2 + 135, 105 });
                    if (ImGui::HighlightedButton("URL", ImVec2{ 100, 15 }, !dllPathGame, ImVec2{ ImGui::GetWindowWidth() / 2 + 139, 107 }, ImVec2{ 96, 13 }, ImVec4{ 255 / 255, 255 / 255, 255 / 255, 0.3f }))
                        dllPathGame = false;

                    if (dllPathGame)
                    {
                        ImGui::SetCursorPos(ImVec2{ 15, 135 });
                        ImGui::Text("DllPath");
                        ImGui::SameLine();
                        ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowWidth() / 2 - 95, 135 });
                        ImGui::PushID(7);
                        ImGui::InputText(" ", dllPath, MAX_PATH, ImVec2{ 330, 15 });
                        ImGui::PopID();
                    }

                    if (!dllPathGame)
                    {
                        ImGui::SetCursorPos(ImVec2{ 15, 135 });
                        ImGui::Text("Download URL");
                        ImGui::SameLine();
                        ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowWidth() / 2 - 95, 135 });
                        ImGui::PushID(7);
                        ImGui::InputText(" ", downloadURL, MAX_PATH, ImVec2{ 330, 15 });
                        ImGui::PopID();

                        ImGui::SetCursorPos(ImVec2{ 15, 165 });
                        ImGui::Text("Download Name");
                        ImGui::SameLine();
                        ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowWidth() / 2 - 95, 165 });
                        ImGui::PushID(8);
                        ImGui::InputText(" ", downloadName, MAX_PATH, ImVec2{ 100, 15 });
                        ImGui::PopID();
                    }

                    ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowWidth() / 2 + 15, 165 });
                    if (ImGui::Button("AddGame", ImVec2{ 100, 15 }))
                    {
                        if (*gameName == '\0')
                            strcpy_s(gameName, 5, "None");

                        if (*processName == '\0')
                            strcpy_s(processName, 5, "None");

                        if (*graphicsEngine == '\0')
                            strcpy_s(graphicsEngine, 5, "None");

                        if (*gameEngine == '\0')
                            strcpy_s(gameEngine, 5, "None");

                        if (*antiCheat == '\0')
                            strcpy_s(antiCheat, 5, "None");

                        if (*obfuscator == '\0')
                            strcpy_s(obfuscator, 5, "None");

                        if (*packer == '\0')
                            strcpy_s(packer, 5, "None");

                        if (dllPathGame)
                        {
                            if (*dllPath == '\0')
                                strcpy_s(dllPath, 5, "None");
                        }

                        if (!dllPathGame)
                        {
                            if (*downloadURL == '\0')
                                strcpy_s(downloadURL, 5, "None");
                            if (*downloadName == '\0')
                                strcpy_s(downloadName, 5, "None");
                        }

                        if (dllPathGame)
                            gameManager.gameList.push_back(Game{ gameName, processName, graphicsEngine, gameEngine, antiCheat, obfuscator, packer, dllPath });
                        else
                            gameManager.gameList.push_back(Game{ gameName, processName, graphicsEngine, gameEngine, antiCheat, obfuscator, packer, downloadURL, downloadName });
                        addGame->openGUI = false;
                    }

                    ImGui::SetCursorPos(ImVec2{ ImGui::GetWindowWidth() / 2 + 135, 165 });
                    if (ImGui::Button("Exit", ImVec2{ 100, 15 }))
                        addGame->openGUI = false;

                    ImGui::EndChild();
                }
                ImGui::PopStyleColor();
            }

            ImGui::PopStyleColor(5);
            ImGui::PopFont();
            ImGui::End();
        }
        //End Draw
        ImGui::EndFrame();

        mainWindow->d3d9device->BeginScene();

        mainWindow->d3d9device->Clear(NULL, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 255, 255), 1, 0);

        ImGui::Render();

        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

        mainWindow->d3d9device->EndScene();

        mainWindow->d3d9device->Present(NULL, NULL, NULL, NULL);
    }
    config.SaveConfig();
    ExitThread(0);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    #if _DEBUG
    AllocConsole();
    FILE* f = new FILE;
    freopen_s(&f, "CONOUT$", "w", stdout);
    #endif

    ImGuiD3d9Window mainWindow{ hInstance, "MenuName", "ClassName", "Cheat Manager", (int)menu::windowResolution.x, (int)menu::windowResolution.y, WindowProcedure };
    if (!mainWindow.RegisterCreateAndShowWindow())
    {
        MessageBoxW(NULL, L"Couldnt Register/Create/Show Window", L"Cheat Manager", MB_OK);
        return 0;
    }

    if (!mainWindow.CreateD3d9Device())
    {
        MessageBoxW(NULL, L"Couldnt Create D3d9Device", L"Cheat Manager", MB_OK);
        return 0;
    }

    if (!mainWindow.InitializeImGui())
    {
        MessageBoxW(NULL, L"Couldnt Initialize ImGui", L"Cheat Manager", MB_OK);
        return 0;
    }

    HANDLE hWindowThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Window, &mainWindow, 0, nullptr);
    if (!hWindowThread)
    {
        MessageBoxW(NULL, L"Couldnt Create Thread", L"Cheat Manager", MB_OK);
        return 0;
    }

    MSG msg;
    while (!shouldExit)
    {
        GetMessageA(&msg, NULL, NULL, NULL);
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    WaitForSingleObject(hWindowThread, INFINITE);
    CloseHandle(hWindowThread);

    mainWindow.ShutDownImGui();

    mainWindow.ReleaseD3d9();

    #if _DEBUG
    FreeConsole();
    fclose(f);
    #endif
	return 0;
}