@echo off
setlocal

REM 用法: build_run.bat <target>
if "%~1"=="" (
    echo Usage: build_run.bat ^<target^>
    echo Example: build_run.bat DX11Texture
    exit /b 1
)

set TARGET=%~1
set CONFIG=Debug
set BUILD_DIR=build

REM 生成构建系统（如未生成）
if not exist "%BUILD_DIR%\CMakeCache.txt" (
    cmake -S . -B "%BUILD_DIR%" || exit /b 1
)

REM 构建指定 target
cmake --build "%BUILD_DIR%" --config %CONFIG% --target %TARGET% || exit /b 1

REM 运行可执行（按 Examples 目录结构）
set EXE_PATH=%BUILD_DIR%\%CONFIG%\Examples\%TARGET%.exe
if not exist "%EXE_PATH%" (
    echo Executable not found: %EXE_PATH%
    echo If your output path differs, adjust EXE_PATH in this script.
    exit /b 1
)

echo Running: %EXE_PATH%
"%EXE_PATH%"