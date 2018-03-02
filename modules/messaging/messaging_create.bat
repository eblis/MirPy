@echo off
python ..\pyrexc.py pyrex\messaging.pyx
echo Running cleanup ...
python ..\cleanup.py pyrex\messaging.c .\