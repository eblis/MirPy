@echo off
python ..\pyrexc.py pyrex\protocols.pyx
echo Running cleanup ...
python ..\cleanup.py pyrex\protocols.c .\