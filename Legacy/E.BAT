@echo off
if "%1"=="2" goto altern
goto norm

:norm
edit /h tsp.c problem.c path.c screen.c sys.c problem.h path.h sys.h out
goto end

:altern
edit /h problem.c problem.h tsp.c file.c path.c screen.c sys.c
goto end

:end
