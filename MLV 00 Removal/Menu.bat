@echo off
title MLV 00 Removal Menu

:: Paths to your executables
set "ERROR_FINDER=%~dp0MLV_Error_Finder\bin\Release\MLV_Error_Finder.exe"
set "TRUCK=%~dp0MLV_Truck\bin\Release\MLV_Truck.exe"

:MENU
cls
echo ========================================
echo          MLV 00 Removal Menu
echo ========================================
echo.
echo 1. Error Finder (check for invalid blockSize '0')
echo 2. MLV Truck (remove trailing 00 bytes at EOF)
echo 3. Exit
echo.
set /p choice=Enter your choice [1-3]:

if "%choice%"=="1" (
    echo Running Error Finder...
    echo Note: This program only checks for errors, it does NOT modify the file.
    pause
    "%ERROR_FINDER%"
    pause
    goto MENU
)

if "%choice%"=="2" (
    echo Running MLV Truck...
    echo Note: This program fixes trailing 00 bytes at the end of the file.
    pause
    "%TRUCK%"
    pause
    goto MENU
)

if "%choice%"=="3" (
    echo Exiting...
    exit /b
)

echo Invalid choice. Try again.
pause
goto MENU
