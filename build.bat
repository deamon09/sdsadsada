@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo.
echo ========================================
echo   DayZ Steam ESP - Build Script
echo ========================================
echo.

REM Procurar MSBuild em locais comuns
set "MSBUILD="

echo Looking for MSBuild...
echo.

REM VS 2022 Community
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
    echo Found: VS 2022 Community
    goto :compile
)

REM VS 2022 Professional
if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"
    echo Found: VS 2022 Professional
    goto :compile
)

REM VS 2022 Enterprise
if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
    echo Found: VS 2022 Enterprise
    goto :compile
)

REM VS 2019 Community
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"
    echo Found: VS 2019 Community
    goto :compile
)

REM VS 2019 Professional
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD=C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\MSBuild\Current\Bin\MSBuild.exe"
    echo Found: VS 2019 Professional
    goto :compile
)

REM VS 2017
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe" (
    set "MSBUILD=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe"
    echo Found: VS 2017
    goto :compile
)

REM Tentar usar vswhere
if exist "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" (
    echo Using vswhere to locate...
    for /f "usebackq tokens=*" %%i in (`"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
        set "MSBUILD=%%i"
        echo Found via vswhere
        goto :compile
    )
)

REM Não encontrado
echo.
echo MSBuild not found!
echo.
echo Options:
echo    1. Install Visual Studio 2019 or 2022
echo    2. Install Build Tools for Visual Studio
echo    3. Build manually in Visual Studio IDE
echo.
echo Download: https://visualstudio.microsoft.com/downloads/
echo.
pause
exit /b 1

:compile
echo.
echo ========================================
echo Cleaning previous build...
echo ========================================
echo.

"%MSBUILD%" "DayZ Steam.sln" /t:Clean /p:Configuration=Release /p:Platform=x64 /v:minimal /nologo

echo.
echo ========================================
echo Building Release x64...
echo ========================================
echo.

"%MSBUILD%" "DayZ Steam.sln" /t:Build /p:Configuration=Release /p:Platform=x64 /v:normal /nologo /m /fl /flp:logfile=build.log;verbosity=detailed

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo Build completed successfully!
    echo ========================================
    echo.
    
    if exist "x64\Release\Steam.exe" (
        echo Output: x64\Release\Steam.exe
        
        for %%A in ("x64\Release\Steam.exe") do (
            set size=%%~zA
            set /a sizeMB=!size! / 1048576
            echo Size: !sizeMB! MB (%%~zA bytes)
        )
        
        echo.
        echo Usage:
        echo    1. Start DayZ and join a server
        echo    2. Run x64\Release\Steam.exe as Administrator
        echo    3. Press INSERT to toggle menu
        echo.
        
        if exist "build.log" (
            echo Build log saved to: build.log
        )
    ) else (
        echo Executable not created!
        echo Check build.log for details
    )
) else (
    echo.
    echo ========================================
    echo Build failed!
    echo ========================================
    echo.
    echo Build log saved to: build.log
    echo.
    echo Common issues:
    echo    - DirectX SDK not installed
    echo    - Missing .cpp files in project
    echo    - Headers not found
    echo    - Libraries not linked
    echo.
    echo Try building in Visual Studio IDE for better error messages
    echo.
)

echo.
pause
