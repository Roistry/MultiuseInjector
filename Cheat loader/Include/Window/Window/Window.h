#pragma once
#include <Windows.h>

class Window
{
protected:
    Window()
    {

    }
public:
    HINSTANCE hInstance;

    const char* menuName;
    const char* className;
    const char* windowName;

    int width;
    int height;

    WNDPROC windowProcedure;

    HWND hwnd;

    Window(HINSTANCE hInstance, const char* menuName, const char* className, const char* windowName, int width, int height, WNDPROC windowProcedure)
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

    bool RegisterCreateAndShowWindow()
    {
        WNDCLASSEXA wc;
        {
            ZeroMemory(&wc, sizeof(wc));
            wc.cbSize = sizeof(wc);
            wc.style = CS_HREDRAW | CS_OWNDC;
            wc.lpfnWndProc = windowProcedure;
            wc.cbClsExtra = NULL;
            wc.cbWndExtra = NULL;
            wc.hInstance = hInstance;
            wc.hIcon = NULL;
            wc.hCursor = NULL;
            wc.hbrBackground = NULL;
            wc.lpszMenuName = menuName;
            wc.lpszClassName = className;
            wc.hIconSm = NULL;
        }
        RegisterClassExA(&wc);

        hwnd = CreateWindowExA(NULL, className, windowName, WS_CAPTION | WS_SYSMENU, ((1920 / 2) - (width / 2)), ((1080 / 2) - (height / 2)), width, height, NULL, NULL, hInstance, &wc);
        if (!hwnd)
            return false;

        ShowWindow(hwnd, 1);

        return true;
    }
};