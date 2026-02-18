@echo off
setlocal enabledelayedexpansion

REM Path to GCC
set GCC=C:\msys64\ucrt64\bin\gcc.exe

REM Directories
set SRC_DIR=src
set BIN_DIR=bin
set BUILD_DIR=build

REM Create directories
if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

REM Compile all C files
echo Compiling...
for %%f in (%SRC_DIR%\*.c) do (
    echo Compiling %%f
    "%GCC%" -c "%%f" -o "%BUILD_DIR%\%%~nf.o" -Wall -Wextra -O2 -std=c99
    if errorlevel 1 (
        echo Compilation error!
        exit /b 1
    )
)

REM Link with Windows libraries
echo Linking...
"%GCC%" %BUILD_DIR%\*.o -o "%BIN_DIR%\raytracer.exe" -lm -luser32 -lgdi32 -Wall -Wextra -O2
if errorlevel 1 (
    echo Linking error!
    exit /b 1
)

echo Build complete!
echo Run with: %BIN_DIR%\raytracer.exe
