@echo off
if "%1"=="" goto bad
goto good

:good
arj a e:\project\bkps\tsp%1 *.c *.h
goto end

:bad
echo What about a file number ???
echo ----------------------------
dir e:\project\bkps\tsp*.arj /b /on
goto end

:end
