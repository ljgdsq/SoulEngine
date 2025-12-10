@echo off
REM filepath: d:\SoulEngine\clean_cso.bat
setlocal enabledelayedexpansion
pushd "%~dp0"

set COUNT=0
for /R %%F in (*.cso) do (
  del /F /Q "%%F" && set /A COUNT+=1
)

echo Deleted !COUNT! .cso files.
popd
endlocal