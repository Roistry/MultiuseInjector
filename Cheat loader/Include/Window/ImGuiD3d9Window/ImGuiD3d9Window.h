#pragma once
#include "Window/D3d9Window/D3d9Window.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx9.h"

class ImGuiD3d9Window : public D3d9Window
{
public:
    ImGuiD3d9Window(HINSTANCE hInstance, const char* menuName, const char* className, const char* windowName, int width, int height, WNDPROC windowProcedure)
    {
        this->hInstance = hInstance;

        this->menuName = menuName;
        this->className = className;
        this->windowName = windowName;

        this->width = width;
        this->height = height;

        this->windowProcedure = windowProcedure;

        this->hwnd = nullptr;
    }

    bool InitializeImGui()
    {
        ImGui::SetCurrentContext(ImGui::CreateContext());

        if (!ImGui::GetCurrentContext)
            return false;

        if (!ImGui_ImplWin32_Init(hwnd))
            return false;

        if (!ImGui_ImplDX9_Init(d3d9device))
            return false;

        return true;
    }

    void ShutDownImGui()
    {
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
    }
};