#ifndef UNICODE
#define UNICODE
#endif

#pragma warning(push, 3)
#include <Windows.h>
#include <dwmapi.h>
#pragma warning(pop)

#include <stdint.h>
#include <stdio.h>

#define true 1
#define false 0

#define internal        static
#define global_variable static

typedef int8_t s8;
typedef uint8_t u8;

typedef int16_t s16;
typedef uint16_t u16;

typedef int32_t s32;
typedef uint32_t u32;

typedef int64_t s64;
typedef uint64_t u64;

typedef int32_t bool32;

global_variable bool32 IsRunning;

internal LRESULT CALLBACK Win32WindowCallback(HWND Window,
                                              UINT Message,
                                              WPARAM WParam,
                                              LPARAM LParam)
{
    LRESULT Result = 0;

    switch(Message)
    {
        case WM_DESTROY:
        case WM_CLOSE:
        {
            IsRunning = false;
        }
        break;

        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        }
        break;
    }

    return Result;
}

int CALLBACK WinMain(HINSTANCE Instance,
                     HINSTANCE PrevInstance,
                     LPSTR CommandLine,
                     int ShowCode)
{
    UNREFERENCED_PARAMETER(PrevInstance);
    UNREFERENCED_PARAMETER(CommandLine);
    UNREFERENCED_PARAMETER(ShowCode);

    WNDCLASSEXW WindowClass   = {0};
    WindowClass.cbSize        = sizeof(WNDCLASSEXW);
    WindowClass.style         = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc   = Win32WindowCallback;
    WindowClass.lpszClassName = L"MainWindow";
    WindowClass.hInstance     = Instance;

    ATOM Atom = RegisterClassEx(&WindowClass);
    if(Atom == 0)
    {
        fprintf(stderr, "RegisterClassEx failed\n");
        exit(1);
    }

    HWND Window = CreateWindowEx(0, L"MainWindow", L"Tower Defence",
                                 WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                                 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                 CW_USEDEFAULT, 0, 0, Instance, 0);
    if(Window == 0)
    {
        fprintf(stderr, "CreateWindowEx failed\n");
        exit(1);
    }

    // HDC DeviceContext = GetDC(Window);
    IsRunning = true;

    while(IsRunning)
    {
        MSG Message;

        while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
        {
            if(Message.message == WM_QUIT)
            {
                IsRunning = false;
            }

            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }

        //
        // NOTE(paulriddle): Frame rate limiter. Waits for vsync.
        //
        DwmFlush();
    }

    return 0;
}
