call env.bat
set OUTPUT=output\weasel

rmdir /s /q %OUTPUT%
mkdir %OUTPUT%
mkdir %OUTPUT%\data
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
copy release\weasel.ime %OUTPUT%
copy release\WeaselServer.exe %OUTPUT%

pause
