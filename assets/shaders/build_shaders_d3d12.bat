@echo off

:: Settings
set HLSL_SPIRV_PACKAGE_PATH=E:\GitHub\ionengine_ops\assets\shaders
set COMPILER_PATH=C:\"Program Files (x86)"\"Windows Kits"\10\bin\10.0.22000.0\x64
set OUTPUT_PATH=..\..\build\Debug

echo Start of compiling shaders (D3D12)

:: Vertex shaders
for /r %%i in (*_vertex.hlsl) do (
    %COMPILER_PATH%/fxc.exe /T vs_5_0 /WX /E main %%i /Fo %OUTPUT_PATH%/%%~ni.hlsl_bin
)

:: Pixel shaders
for /r %%i in (*_pixel.hlsl) do (
    %COMPILER_PATH%/fxc.exe /T ps_5_0 /WX /E main %%i /Fo %OUTPUT_PATH%/%%~ni.hlsl_bin
)

echo End of compiling shaders (D3D12)

echo Start of packaging

:: HLSL BIN shaders
for /r %%i in (%OUTPUT_PATH%/*.hlsl_bin) do (
    call set "Params=%%Params%% "%OUTPUT_PATH%/%%~ni.hlsl_bin""
)

:: SPIRV BIN shaders
for /r %%i in (%OUTPUT_PATH%/*.spirv_bin) do (
    call set "Params=%%Params%% "%OUTPUT_PATH%/%%~ni.spirv_bin""
)

%HLSL_SPIRV_PACKAGE_PATH%/hlsl_spirv_package.exe -o "%OUTPUT_PATH%/shader_package.hlsv"%Params%

echo End of packaging

echo Delete temp files (HLSL)

:: Vertex shaders
for /r %%i in (%OUTPUT_PATH%/*_vertex.hlsl_bin) do (
    erase "%OUTPUT_PATH%\%%~ni.hlsl_bin"
)

:: Pixel shaders
for /r %%i in (%OUTPUT_PATH%/*_pixel.hlsl_bin) do (
    erase "%OUTPUT_PATH%\%%~ni.hlsl_bin"
)

pause