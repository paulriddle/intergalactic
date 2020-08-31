#include "intergalactic.h"

void RenderDebugGradient(game_backbuffer *Buffer,
                         s32 BlueOffset,
                         s32 GreenOffset)
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

void GameUpdateAndRender(game_backbuffer *Buffer,
                         s32 BlueOffset,
                         s32 GreenOffset)
{
    RenderDebugGradient(Buffer, BlueOffset, GreenOffset);
}
