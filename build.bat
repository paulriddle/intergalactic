@echo off

if not exist build mkdir build
pushd build
cl -nologo -Zi -FC ..\win32_platform.c -Fe:game /link user32.lib gdi32.lib
popd

REM Compiler Flags:

REM Zi  : debug info
REM Zo  : More debug info for optimized builds
REM FC  : Full path on errors, allows 4coder to jump to lines with errors
REM Oi  : Always do intrinsics if possible
REM Od  : No optimizations
REM O2  : Full optimizations
REM MT  : Use the C static lib instead of searching for dll at run-time
REM MTd : Same as MT but using the debug version of CRT
REM GR- : Turn off C++ runtime type info
REM Gm- : Turn off incremental build
REM EHa-: Turn off exception handling
REM WX  : Treat warnings as errors
REM W4  : Set warning level to 4 (Wall to all levels)
REM wd  : Ignore warning
REM Fe  : name of the executable
REM fp:fast    : Ignores the rules in some cases to optimize fp operations
REM Fmfile.map : Outputs a map file (mapping of the functions on the exr)

REM Linker Options:

REM subsystem:windows,5.1 : Make exe compatible with Windows XP (only works on x86)
REM opt:ref               : Don't put unused things in the exe
REM incremental:no        : Don't need to do incremental builds
REM LD                    : Build a dll
REM PDB:file.pdb          : Change the .pdb's path