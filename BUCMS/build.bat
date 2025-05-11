@echo off
gcc -Wall -Wextra -std=c99 src/*.c -o cms.exe
if %errorlevel% equ 0 (
    echo Build successful! Run "cms.exe" to start.
) else (
    echo Build failed. Check for errors.
)
pause