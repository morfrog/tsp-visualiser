@echo off
del view.txt >null
for %%a in (0 1 2 3 4 5 6 7 8 9) do call teste.bat %%a
edit *.txt
