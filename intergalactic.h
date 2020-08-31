struct
{
    void *Memory;
    s32 Width;
    s32 Height;
    s32 Pitch;
} typedef game_backbuffer;

void GameUpdateAndRender(game_backbuffer *Buffer, s32 BlueOffset, s32 GreenOffset);
