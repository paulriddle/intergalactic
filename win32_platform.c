#include "generic_types.h"
#include "intergalactic.c"

#ifndef UNICODE
#define UNICODE
#endif

#pragma warning(push, 3)
#include <Windows.h>
#include <dwmapi.h>
#pragma warning(pop)

#include <intrin.h>
#include <stdio.h>
#include "win32_platform.h"

#define BYTES_PER_PIXEL 4

struct
{
    bool32 IsDown;
    bool32 Changed;
} typedef button;

enum
{
    BUTTON_D,
    BUTTON_COUNT,
};

struct
{
    button Buttons[BUTTON_COUNT];
} typedef input;

#define WasPressed(Button)                                                     \
    (Input.Buttons[Button].IsDown && Input.Buttons[Button].Changed)
#define WasReleased(Button)                                                    \
    (!Input.Buttons[Button].IsDown && Input.Buttons[Button].Changed)
#define IsDown(Button) (Input.Buttons[Button].IsDown)

global_variable bool32 IsRunning;
global_variable bool32 ShowPerformance;
global_variable win32_backbuffer Backbuffer;
global_variable input Input = {0};

win32_window_dimension Win32GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;

    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width  = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;

    return Result;
}

void Win32ResizeDIBSection(win32_backbuffer *Buffer, s32 Width, s32 Height)
{
    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    Buffer->Width  = Width;
    Buffer->Height = Height;

    Buffer->Info.bmiHeader.biSize        = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth       = Width;
    Buffer->Info.bmiHeader.biHeight      = -Height;
    Buffer->Info.bmiHeader.biPlanes      = 1;
    Buffer->Info.bmiHeader.biBitCount    = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;

    s32 BitmapMemorySize = (Width * Height) * BYTES_PER_PIXEL;
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE | MEM_COMMIT,
                                  PAGE_READWRITE);
    if(!Buffer->Memory)
    {
        MessageBox(
            0, L"VirtualAlloc failed to allocate memory for drawing surface",
            L"Error", MB_ICONEXCLAMATION | MB_OK);
        exit(1);
    }
    Buffer->Pitch = Width * BYTES_PER_PIXEL;
}

void Win32DisplayBufferInWindow(HDC DeviceContext,
                                s32 WindowWidth,
                                s32 WindowHeight,
                                win32_backbuffer *Buffer)
{
    StretchDIBits(DeviceContext, 0, 0, WindowWidth, WindowHeight, 0, 0,
                  Buffer->Width, Buffer->Height, Buffer->Memory, &Buffer->Info,
                  DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK Win32WindowCallback(HWND Window,
                                     UINT Message,
                                     WPARAM WParam,
                                     LPARAM LParam)
{
    LRESULT Result = 0;

    switch(Message)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext                = BeginPaint(Window, &Paint);
            win32_window_dimension Dimension = Win32GetWindowDimension(Window);
            Win32DisplayBufferInWindow(DeviceContext, Dimension.Width,
                                       Dimension.Height, &Backbuffer);
            EndPaint(Window, &Paint);
        }
        break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            u32 VKCode = WParam;
            /* bool32 WasDown = (LParam & (1 << 30)) != 0; */
            bool32 IsDown = (LParam & (1 << 31)) == 0;

#define ProcessButton(VirtualButton, Button)                                   \
    if(VKCode == VirtualButton)                                                \
    {                                                                          \
        Input.Buttons[Button].Changed =                                        \
            IsDown != Input.Buttons[Button].IsDown;                            \
        Input.Buttons[Button].IsDown = IsDown;                                 \
    }
            ProcessButton('D', BUTTON_D);

            if(WasPressed(BUTTON_D))
            {
                ShowPerformance = !ShowPerformance;
            }

            bool32 AltKeyWasDown = (LParam & (1 << 29));
            if((VKCode == VK_F4) && AltKeyWasDown)
            {
                IsRunning = false;
            }
        }
        break;

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

    int ExitCode = 0;

    s64 PerfCountFrequency;
    QueryPerformanceFrequency((LARGE_INTEGER *)&PerfCountFrequency);
    real32 Frequency = (real32)PerfCountFrequency;

    Win32ResizeDIBSection(&Backbuffer, 1280, 720);

    WNDCLASSEXW WindowClass   = {0};
    WindowClass.cbSize        = sizeof(WNDCLASSEXW);
    WindowClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc   = Win32WindowCallback;
    WindowClass.lpszClassName = L"MainWindow";
    WindowClass.hInstance     = Instance;
    WindowClass.hCursor       = LoadCursor(0, IDC_ARROW);
    WindowClass.hbrBackground = CreateSolidBrush(RGB(255, 0, 255));

    if(!RegisterClassEx(&WindowClass))
    {
        ExitCode = GetLastError();
        MessageBox(0, L"RegisterClassEx failed\n", L"Error",
                   MB_ICONEXCLAMATION | MB_OK);
        exit(ExitCode);
    }

    HWND Window = CreateWindowEx(0, L"MainWindow", L"Intergalactic",
                                 WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                                 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                 CW_USEDEFAULT, 0, 0, Instance, 0);
    if(!Window)
    {
        ExitCode = GetLastError();
        MessageBox(0, L"CreateWindowEx failed\n", L"Error",
                   MB_ICONEXCLAMATION | MB_OK);
        exit(ExitCode);
    }

    HDC DeviceContext = GetDC(Window);
    if(!DeviceContext)
    {
        MessageBox(0, L"GetDC failed\n", L"Error", MB_ICONEXCLAMATION | MB_OK);
        exit(1);
    }
    HFONT MonospaceFont = (HFONT)GetStockObject(ANSI_FIXED_FONT);
    if(MonospaceFont)
    {
        SelectObject(DeviceContext, MonospaceFont);
    }

#if INTERNAL_BUILD
    LPVOID BaseAddress = (LPVOID)Terabytes(1);
#else
    LPVOID BaseAddress = 0;
#endif

    game_memory GameMemory = {};
    GameMemory.PermanentStorageSize = Megabytes(64);
    GameMemory.TransientStorageSize = Gigabytes(2);
    u64 TotalSize = GameMemory.PermanentStorageSize + GameMemory.TransientStorageSize;
    GameMemory.PermanentStorage = VirtualAlloc(BaseAddress, TotalSize,
                                                   MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if(!GameMemory.PermanentStorage)
    {
        ExitCode = GetLastError();
        MessageBox(0, L"VirtualAlloc failed\n", L"Error",
                       MB_ICONEXCLAMATION | MB_OK);
        exit(ExitCode);
    }
    GameMemory.TransientStorage = (byte *)GameMemory.PermanentStorage +
        GameMemory.PermanentStorageSize;

    IsRunning = true;

    s64 LastFrame;
    QueryPerformanceCounter((LARGE_INTEGER *)&LastFrame);
    u64 LastCycleCount = __rdtsc();
    while(IsRunning)
    {
        for(s32 Button = 0; Button < BUTTON_COUNT; Button++)
        {
            Input.Buttons[Button].Changed = false;
        }

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

        game_backbuffer GameBackbuffer = {};
        GameBackbuffer.Memory = Backbuffer.Memory;
        GameBackbuffer.Width = Backbuffer.Width;
        GameBackbuffer.Height = Backbuffer.Height;
        GameBackbuffer.Pitch = Backbuffer.Pitch;
        GameUpdateAndRender(&GameMemory, &GameBackbuffer);
        win32_window_dimension Dimension = Win32GetWindowDimension(Window);
        Win32DisplayBufferInWindow(DeviceContext, Dimension.Width,
                                   Dimension.Height, &Backbuffer);

        if(ShowPerformance)
        {
            u64 CurrentCycleCount = __rdtsc();
            s64 CurrentFrame;
            QueryPerformanceCounter((LARGE_INTEGER *)&CurrentFrame);

            u64 CyclesElapsed  = CurrentCycleCount - LastCycleCount;
            s64 CounterElapsed = CurrentFrame - LastFrame;

            real32 MegacyclesPerFrame =
                (real32)CyclesElapsed / (1000.0f * 1000.0f);
            real32 MillisecondsPerFrame =
                1000.0f * (real32)CounterElapsed / Frequency;
            real32 FPS = Frequency / (real32)CounterElapsed;

            wchar_t PerformanceInfo[256] = {0};
            swprintf_s(PerformanceInfo,
                       sizeof(PerformanceInfo) / sizeof(wchar_t),
                       L"%.02f milliseconds/frame, %.02f FPS, %.02f "
                       L"megacycles/frame\n",
                       MillisecondsPerFrame, FPS, MegacyclesPerFrame);
            TextOut(DeviceContext, 0, 0, PerformanceInfo,
                    wcslen(PerformanceInfo));

            LastFrame      = CurrentFrame;
            LastCycleCount = CurrentCycleCount;
        }
    }

    return ExitCode;
}
