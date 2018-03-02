@echo off
xcopy "release (static)\*.dll" "temp (static)\plugins\" /Y
xcopy docs\*.* "temp (static)\docs\" /Y
xcopy docs\MirPy\*.* "temp (static)\docs\MirPy\" /Y
for /D %%i in (modules\*) do xcopy "%%i\release (static)\*.pyd" "temp (static)\scripts\MirPy\modules\" /Y

for /F "tokens=5-9* delims=. " %%i in (docs\MirPy_readme.txt) do (call :Pack %%i %%j %%k %%l; exit)

:Pack
d:\usr\PowerArchiver\pacl\pacomp.exe -p -r -a -c2 "MirPy %1.%2.%3.%4 (static).zip" "temp (static)\*.*"
call "pack symbols.bat" MirPy "Release (static)" %1.%2.%3.%4  static
exit

error:
echo "Error packing MirPy"
