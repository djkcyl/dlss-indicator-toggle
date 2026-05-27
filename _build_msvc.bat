@echo off
set "PATH=C:\Program Files (x86)\Microsoft Visual Studio\Installer;%PATH%"
echo Script dir: %~dp0
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
echo After vcvars cwd: %CD%
cd /d "%~dp0"
echo After cd cwd: %CD%
dir /b build.bat
call "%~dp0build.bat"
