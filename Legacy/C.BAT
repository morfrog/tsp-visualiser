@echo off
if "%1"=="l" goto list
cls
echo Compiling...%1 Methods
if exist "%2.exe" del %2.exe
wmake -f e:\watcom\tsp\%3.mk -h -e >out
find /I "ERROR" out >errors
find /I /V "ERROR" out | find /I /V "wcc386" >warning
goto list

:list
cls
echo.
if exist "%2.exe" goto ok
echo ------------------%1 Compilation Failure-----------------------------
goto lister

:ok
echo -------------------%1 Compiled OK-------------------------------------
goto lister

:lister
echo.
type warning
type errors


