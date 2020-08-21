#include <windows.h>

#define internal static
#define global_variable static

global_variable bool GlobalRunning;

internal LRESULT CALLBACK
Win32WindowCallback(HWND Window,
                    UINT Message,
                    WPARAM WParam,
                    LPARAM LParam)
{
}

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{
    WNDCLASSA WindowClass = {
        .style = CS_HREDRAW|CS_VREDRAW,
        .lpfnWndProc = Win32WindowCallback,
        .lpszClassName = "TowerDefenceWindowClass",
        .hInstance = Instace
    };
    
    if(RegisterClassA(&WindowClass))
    {
        HWND Window = CreateWindowExA(0,
                                      WindowClass.lpszClassName,
                                      "Tower Defence",
                                      WS_VISIBLE|WS_OVERLAPPEDWINDOW,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      1280,
                                      720,
                                      0,
                                      0,
                                      Instance,
                                      0);
        if(Window)
        {
            HDC DeviceContext = GetDC(Window);
            
            GlobalRunning = true;
            
            while(GlobalRunning)
            {
                MSG Message;
                while(PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
                {
                    // TODO(paulriddle): Rewrite using modern Windows API
                }
            }
        }
        else
        {
            OutputDebugStringA("CreateWindowExA failed\n");
            exit(1);
        }
    }
    else
    {
        OutputDebugStringA("RegisterClassA failed\n");
        exit(1);
    }
}