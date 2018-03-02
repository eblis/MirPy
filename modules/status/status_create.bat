@echo off
python ..\pyrexc.py pyrex\status.pyx
echo Running cleanup ...
python ..\cleanup.py pyrex\status.c .\