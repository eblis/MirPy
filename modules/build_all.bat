@echo off
for /D %%f in (*.*) do (call :processFolder %%f) 

goto :end

:processFolder
if not %i == "" (
echo "Building %1 module ..."
cd %1
call %1_create.bat
cd ..
)

:end
