@echo off
setlocal

set "BUILD_DIR=build"

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

echo Configuring CMake in "%BUILD_DIR%" ...
REM 透传其它自定义参数，例如 -G "Visual Studio 17 2022" -A x64
cmake -S . -B "%BUILD_DIR%" %*
if errorlevel 1 exit /b 1

echo Done.
endlocal