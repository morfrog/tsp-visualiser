@echo off
if "%1"=="2" goto altern
if "%1"=="3" goto rndfile
goto norm

:norm
edit tspann.c anneal.c anneal.h problem.c optim.c sys.h problem.h path.h out
goto end

:altern
edit /h problem.c problem.h tsp.c file.c path.c screen.c sys.c
goto end

:rndfile
edit /h rndfile.c file.c sys.c file.h sys.h
goto end

:end
