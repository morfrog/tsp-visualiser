@echo off
if "%1"=="" goto all
goto single

echo Compiling sys.c...
wcc386 sys.c -i=e:\WATCOM\h -w4 -e25 -zq -otexan -d2 -hw -fp5 -fpi87 -mm -5r -bt=dos >>out
echo Compiling file.c...
wcc386 file.c -i=e:\WATCOM\h -w4 -e25 -zq -otexan -d2 -hw -fp5 -fpi87 -mm -5r -bt=dos >>out
echo Compiling screen.c...
wcc386 screen.c -i=e:\WATCOM\h -w4 -e25 -zq -otexan -d2 -hw -fp5 -fpi87 -mm -5r -bt=dos >>out
echo Compiling path.c...
wcc386 path.c -i=e:\WATCOM\h -w4 -e25 -zq -otexan -d2 -hw -fp5 -fpi87 -mm -5r -bt=dos >>out
echo Compiling tsp.c...
wcc386 tsp.c -i=e:\WATCOM\h -w4 -e25 -zq -otexan -d2 -hw -fp5 -fpi87 -mm -5r -bt=dos >>out


:all
cls
echo Compiling...
wmake -f e:\watcom\tsp\main.mk -h -e >out
type out
echo --------------------------------------------------------------------------------
find /I /V "SYMBOL" out | find /I /V "wlink" | find /I /V "wcc386" | find /I /V "cd e:"
goto end

:single
cls
echo Compiling %1.c
wcc386 %1.c -i=e:\WATCOM\h -w4 -e25 -zq -otexan -d2 -fp5 -mm -5r -bt=dos
goto end

:end
