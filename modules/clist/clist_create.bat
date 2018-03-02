@echo off
python ..\pyrexc.py pyrex\clist.pyx
echo Running cleanup ...
python ..\cleanup.py pyrex\clist.c .\