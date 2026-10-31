@echo off
setlocal
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0Abominable.ps1" -Action SetupRegistry
echo.
pause
