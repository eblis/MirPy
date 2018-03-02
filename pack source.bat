for /F "tokens=5-9* delims=. " %%i in (docs\MirPy_readme.txt) do (call :Pack %%i %%j %%k %%l; exit)

:Pack
d:\usr\PowerArchiver\pacl\pacomp.exe -p -a -c2 "MirPy src %1.%2.%3.%4.zip" @files_source.txt -x*.zip -x*.ncb -x*.user -x*.aps
exit

error:
echo "Error packing MirPy"
