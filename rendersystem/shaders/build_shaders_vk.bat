@echo off

:: Settings
set COMPILER_PATH=%VULKAN_SDK%\Bin
set OUTPUT_PATH=D:\GitHub\ionengine_ops\build\Debug\shaders\vk

echo Start of compiling shaders (Vulkan)

:: Vertex shaders
for /r %%i in (*_vert.hlsl) do (
    %COMPILER_PATH%/glslc.exe -fshader-stage=vertex %%i -o %OUTPUT_PATH%/%%~ni.bin
)

:: Fragment shaders
for /r %%i in (*_frag.hlsl) do (
    %COMPILER_PATH%/glslc.exe -fshader-stage=fragment %%i -o %OUTPUT_PATH%/%%~ni.bin
)

echo End of compiling shaders (Vulkan)

pause