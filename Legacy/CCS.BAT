@echo off
if "%1"=="l" goto list
cls
echo Compiling...Compare Solutions
if exist "compsoln.exe" del compsoln.exe
wmake -f e:\watcom\tsp\compsoln.mk -h -e >out
find /I "ERROR" out >errors
find /I /V "ERROR" out | find /I /V "wcc386" >warning
goto list

:list
cls
echo.
if exist "compsoln.exe" goto ok
echo --------------------------Compilation Failure-----------------------------
goto lister

:ok
echo --------------------------Compiled OK-------------------------------------
goto lister

:lister
echo.
type warning
type errors


