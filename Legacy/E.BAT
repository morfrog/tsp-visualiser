@echo off
if "%1"=="2" goto altern
goto norm

:norm
edit /h tsp.c file.c path.c screen.c sys.c file.h path.h screen.h sys.h
goto end

:altern
edit /h problem.c problem.h tsp.c file.c path.c screen.c sys.c
goto end

:end
