#ifndef WIN32_PLATFORM_H
#define WIN32_PLATFORM_H

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

#endif //WIN32_PLATFORM_H
