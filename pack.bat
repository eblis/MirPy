@echo off
xcopy release\*.dll temp\plugins\ /Y
xcopy docs\*.* temp\docs\ /Y
xcopy docs\MirPy\*.* temp\docs\MirPy\ /Y
for /D %%i in (modules\*) do xcopy %%i\release\*.pyd temp\scripts\MirPy\modules\ /Y

for /F "tokens=5-9* delims=. " %%i in (docs\MirPy_readme.txt) do (call :Pack %%i %%j %%k %%l; exit)

:Pack
d:\usr\PowerArchiver\pacl\pacomp.exe -p -r -a -c2 "MirPy %1.%2.%3.%4.zip" temp\*.*
call "pack symbols.bat" MirPy Release %1.%2.%3.%4
exit

error:
echo "Error packing MirPy"
