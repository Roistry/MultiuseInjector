#include "imimgui.h"

void ImGui::SelectablesList(const char* nameList[], size_t nameSize, ImVec2 pos, ImVec2 buttonSize, int* selected, bool* listOpen)
{
    ImGui::SetCursorPos(pos);
    if (ImGui::Button(nameList[*selected], buttonSize))
        *listOpen = !*listOpen;

    if (*listOpen)
    {
        for (size_t i = 0; i < nameSize; i++)
        {
            ImGui::SetCursorPos(ImVec2(pos.x, pos.y + i * buttonSize.y + 5 + buttonSize.y));
            ImGui::PushID(2);
            if (ImGui::Button(nameList[i], ImVec2{ buttonSize.x, buttonSize.y }))
            {
                *selected = i;
                *listOpen = false;
            }
            ImGui::PopID();
        }
    }
}

void ImGui::SelectablesList(std::vector<const char*> vecNames, ImVec2 pos, ImVec2 buttonSize, int* selected, bool* listOpen)
{
    ImGui::SetCursorPos(pos);
    if (ImGui::Button(vecNames[*selected], buttonSize))
        *listOpen = !*listOpen;

    if (*listOpen)
    {
        for (size_t i = 0; i < vecNames.size(); i++)
        {
            ImGui::SetCursorPos(ImVec2(pos.x, pos.y + i * buttonSize.y + 5 + buttonSize.y));
            ImGui::PushID(2);
            if (ImGui::Button(vecNames[i], ImVec2{ buttonSize.x, buttonSize.y }))
            {
                *selected = i;
                *listOpen = false;
            }
            ImGui::PopID();
        }
    }
}

bool ImGui::HighlightedButton(const char* label, const ImVec2& size_arg, bool highlighted, ImVec2 highlightStartingPos, ImVec2 highlightSize, ImVec4 highlightColor)
{
    ImGui::PushStyleColor(ImGuiCol_Header, highlightColor);
    bool buttonRet = ImGui::Button(label, size_arg);
    ImVec2 fixedHighlightStartingPos = ImVec2{ highlightStartingPos.x - 4, highlightStartingPos.y - 2 };
    ImGui::SetCursorPos(highlightStartingPos);
    ImVec2 fixedHighlightSize = ImVec2{ highlightSize.x - 4, highlightSize.y - 2 };
    ImGui::Selectable(" ", highlighted, 0, fixedHighlightSize);
    ImGui::PopStyleColor();

    return buttonRet;
}

bool ImGui::HighlightedButton(const char* label, const ImVec2& size_arg, bool* highlighted, ImVec2 highlightStartingPos, ImVec2 highlightSize, ImVec4 highlightColor)
{
    ImGui::PushStyleColor(ImGuiCol_Header, highlightColor);
    bool buttonRet = ImGui::Button(label, size_arg);
    ImGui::SetCursorPos(highlightStartingPos);
    ImVec2 fixedHighlightSize = ImVec2{ highlightSize.x - 4, highlightSize.y - 2 };
    ImGui::Selectable(" ", highlighted, 0, fixedHighlightSize);
    ImGui::PopStyleColor();

    return buttonRet;
}