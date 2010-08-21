call env.bat
del /q output\*
rmdir /s /q output\engine
copy release\weasel.ime output
copy release\WeaselServer.exe output
copy lib\boost_python-vc90-mt-1_43.dll output
mkdir output\engine
copy engine\*.py output\engine
mkdir output\engine\ibus
copy engine\ibus\*.py output\engine\ibus
copy misc\*.conf output\engine
copy misc\*.js output
copy misc\*.bat output
pause
