@echo off
build.bat
if %errorlevel% neq 0 exit /b

bin\Debug\AncientMythosGame.exe > logs.txt 2>&1
