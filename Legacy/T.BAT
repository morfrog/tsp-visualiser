@echo off
if "%1"=="" goto nopar
goto par

:nopar
mapmake 5 | tsp
goto end

:par
mapmake %1 %2| tsp
goto end

:end
