call env.bat
set OUTPUT=output\weasel

mkdir %OUTPUT%
del /q %OUTPUT%\*
rmdir /s /q %OUTPUT%\data
rmdir /s /q %OUTPUT%\engine
rmdir /s /q %OUTPUT%\Microsoft.VC90.CRT
copy release\weasel.ime %OUTPUT%
copy release\WeaselServer.exe %OUTPUT%
copy lib\boost_python-vc90-mt-1_43.dll %OUTPUT%
mkdir %OUTPUT%\data
copy data\*.bat %OUTPUT%\data
copy data\*.py %OUTPUT%\data
copy data\*.txt %OUTPUT%\data
mkdir %OUTPUT%\engine
copy engine\*.py %OUTPUT%\engine
mkdir %OUTPUT%\engine\ibus
copy engine\ibus\*.py %OUTPUT%\engine\ibus
copy misc\*.bat %OUTPUT%
copy misc\*.conf %OUTPUT%\engine
copy misc\*.js %OUTPUT%
copy misc\*.txt %OUTPUT%
mkdir %OUTPUT%\Microsoft.VC90.CRT
copy misc\Microsoft.VC90.CRT\* %OUTPUT%\Microsoft.VC90.CRT

pause
