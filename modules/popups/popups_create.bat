@echo off
python ..\pyrexc.py pyrex\popups.pyx
echo Running cleanup ...
python ..\cleanup.py pyrex\popups.c .\