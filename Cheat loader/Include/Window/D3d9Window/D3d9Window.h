#pragma once
#include <d3d9.h>

#include "Window/Window/Window.h"

#pragma comment(lib, "d3d9.lib")

class D3d9Window : public Window
{
protected:
    D3d9Window()
    {
        d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
        d3d9device = nullptr;
    }
public:
    IDirect3D9* d3d9;
    IDirect3DDevice9* d3d9device;

    D3d9Window(HINSTANCE hInstance, const char* menuName, const char* className, const char* windowName, int width, int height, WNDPROC windowProcedure)
    {
        this->hInstance = hInstance;

        this->menuName = menuName;
        this->className = className;
        this->windowName = windowName;

        this->width = width;
        this->height = height;

        this->windowProcedure = windowProcedure;

        this->hwnd = nullptr;

        d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
        d3d9device = nullptr;
    }

    bool CreateD3d9Device()
    {
        D3DPRESENT_PARAMETERS d3dpp;
        {
            ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
            d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
            d3dpp.Windowed = true;
            d3dpp.hDeviceWindow = hwnd;
        }
        if (FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3d9device)))
            return false;

        return true;
    }

    void ReleaseD3d9()
    {
        d3d9device->Release();
        d3d9->Release();
    }
};