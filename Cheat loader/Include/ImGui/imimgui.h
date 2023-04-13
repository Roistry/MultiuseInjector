#pragma once
#include <vector>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx9.h"

namespace ImGui
{
    void SelectablesList(const char* nameList[], size_t nameSize, ImVec2 pos, ImVec2 buttonSize, int* selected, bool* listOpen);

    void SelectablesList(std::vector<const char*> vecNames, ImVec2 pos, ImVec2 buttonSize, int* selected, bool* listOpen);

    bool HighlightedButton(const char* label, const ImVec2& size_arg, bool highlighted, ImVec2 highlightStartingPos, ImVec2 highlightSize, ImVec4 highlightColor);

    bool HighlightedButton(const char* label, const ImVec2& size_arg, bool* highlighted, ImVec2 highlightStartingPos, ImVec2 highlightSize, ImVec4 highlightColor);
}