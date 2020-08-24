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
typedef uint8_t byte;

typedef int16_t s16;
typedef uint16_t u16;

typedef int32_t s32;
typedef uint32_t u32;

typedef int64_t s64;
typedef uint64_t u64;

typedef int32_t bool32;

#define BYTES_PER_PIXEL 4

struct
{
    BITMAPINFO Info;
    void *Memory;
    s32 Width;
    s32 Height;
    s32 Pitch;
} typedef win32_backbuffer;

struct
{
    s32 Width;
    s32 Height;
} typedef win32_window_dimension;

global_variable bool32 IsRunning;
global_variable win32_backbuffer Backbuffer;

win32_window_dimension Win32GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;

    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width  = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;

    return Result;
}

internal void
RenderDebugGradient(win32_backbuffer *Buffer, s32 BlueOffset, s32 GreenOffset)
{
    byte *Row = (byte *)Buffer->Memory;

    for(s32 Y = 0; Y < Buffer->Height; ++Y)
    {
        u32 *Pixel = (u32 *)Row;

        for(s32 X = 0; X < Buffer->Width; ++X)
        {
            byte Blue  = X + BlueOffset;
            byte Green = Y + GreenOffset;
            *Pixel++   = (Green << 8) | Blue;
        }

        Row += Buffer->Pitch;
    }
}

internal void
Win32ResizeDIBSection(win32_backbuffer *Buffer, s32 Width, s32 Height)
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

internal void Win32DisplayBufferInWindow(HDC DeviceContext,
                                         s32 WindowWidth,
                                         s32 WindowHeight,
                                         win32_backbuffer *Buffer)
{
    StretchDIBits(DeviceContext, 0, 0, WindowWidth, WindowHeight, 0, 0,
                  Buffer->Width, Buffer->Height, Buffer->Memory, &Buffer->Info,
                  DIB_RGB_COLORS, SRCCOPY);
}

internal LRESULT CALLBACK Win32WindowCallback(HWND Window,
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
    s32 XOffset       = 0;
    s32 YOffset       = 0;
    IsRunning         = true;

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

        RenderDebugGradient(&Backbuffer, XOffset, YOffset);
        win32_window_dimension Dimension = Win32GetWindowDimension(Window);
        Win32DisplayBufferInWindow(DeviceContext, Dimension.Width,
                                   Dimension.Height, &Backbuffer);
        ++XOffset;
        YOffset += 2;
    }

    return ExitCode;
}
