@echo off
python ..\pyrexc.py pyrex\contacts.pyx
echo Running cleanup ...
python ..\cleanup.py pyrex\contacts.c .\