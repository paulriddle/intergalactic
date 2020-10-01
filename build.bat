@echo off

if not exist build mkdir build
clang win32_platform.c -Wall -Wextra -fdiagnostics-absolute-paths -fuse-ld=lld -g -o build\game.exe -luser32 -lgdi32 -ldwmapi -lkernel32
