@echo off
setlocal
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0Abominable.ps1" -Action PlayBorderless
if errorlevel 1 pause
