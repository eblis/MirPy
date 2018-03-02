@echo off
python ..\pyrexc.py pyrex\database.pyx
echo Running cleanup ...
python ..\cleanup.py pyrex\database.c .\