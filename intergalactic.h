#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

struct
{
    void *Memory;
    s32 Width;
    s32 Height;
    s32 Pitch;
} typedef game_backbuffer;

struct
{
    bool32 IsInitialized;

    u64 PermanentStorageSize;
    void *PermanentStorage;

    u64 TransientStorageSize;
    void *TransientStorage;
} typedef game_memory;

struct
{
    s32 GreenOffset;
    s32 BlueOffset;
} typedef game_state;

void GameUpdateAndRender(game_memory *GameMemory, game_backbuffer *Buffer);
