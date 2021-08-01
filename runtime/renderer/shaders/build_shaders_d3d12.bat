@echo off

:: Settings
set COMPILER_PATH=C:\"Program Files (x86)"\"Windows Kits"\10\bin\10.0.18362.0\x64
set OUTPUT_PATH=D:\GitHub\ionengine_ops\build\Debug\shaders\pc

echo Start of compiling shaders (D3D12)

:: Vertex shaders
for /r %%i in (*_vert.hlsl) do (
    %COMPILER_PATH%/fxc.exe /T vs_5_0 /WX /E main %%i /Fo %OUTPUT_PATH%/%%~ni.bin
)

:: Fragment shaders
for /r %%i in (*_frag.hlsl) do (
    %COMPILER_PATH%/fxc.exe /T ps_5_0 /WX /E main %%i /Fo %OUTPUT_PATH%/%%~ni.bin
)

echo End of compiling shaders (D3D12)

pause