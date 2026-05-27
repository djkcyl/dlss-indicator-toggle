@echo off
setlocal

REM Try MSVC first (cl.exe in PATH means a "Developer Command Prompt")
where cl >nul 2>&1
if %errorlevel%==0 goto :msvc

REM Try MinGW
where g++ >nul 2>&1
if %errorlevel%==0 goto :mingw

REM Try common MSYS2 paths
if exist "C:\msys64\ucrt64\bin\g++.exe" (
    set "PATH=C:\msys64\ucrt64\bin;%PATH%"
    goto :mingw
)
if exist "C:\msys64\mingw64\bin\g++.exe" (
    set "PATH=C:\msys64\mingw64\bin;%PATH%"
    goto :mingw
)

echo No C++ compiler found. Install MSYS2 (winget install MSYS2.MSYS2),
echo then run: pacman -S --noconfirm mingw-w64-ucrt-x86_64-toolchain
exit /b 1

:mingw
echo === MinGW build ===
windres resource.rc -O coff -o resource.res.o || exit /b 1
g++ -municode -std=c++17 -Os -s -static -mwindows ^
    main.cpp resource.res.o -o DlssIndicatorToggle.exe ^
    -luser32 -ladvapi32 -lcomctl32 -lshell32 -lole32 || exit /b 1
del resource.res.o 2>nul
goto :done

:msvc
echo === MSVC build ===
rc /nologo /fo resource.res resource.rc || exit /b 1
cl /nologo /std:c++17 /utf-8 /O1 /Os /MT /D_UNICODE /DUNICODE /EHsc ^
    main.cpp resource.res ^
    /link /SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup ^
    user32.lib advapi32.lib comctl32.lib shell32.lib ole32.lib ^
    /OUT:DlssIndicatorToggle.exe || exit /b 1
del resource.res main.obj 2>nul
goto :done

:done
echo.
for %%I in (DlssIndicatorToggle.exe) do echo Built: %%~zI bytes (%%~fI)
