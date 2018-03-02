@echo off
python ..\pyrexc.py pyrex\mirandamisc.pyx
echo Running cleanup ...
python ..\cleanup.py pyrex\mirandamisc.c .\