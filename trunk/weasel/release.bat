call env.bat
set OUTPUT=output\weasel
set SERVER=%OUTPUT%\WeaselServer.exe

if exist %SERVER% %SERVER% /q
rmdir /s /q %OUTPUT%
mkdir %OUTPUT%
mkdir %OUTPUT%\data
copy ..\rime\data\*.bat %OUTPUT%\data
copy ..\rime\data\*.py %OUTPUT%\data
copy ..\rime\data\*.txt %OUTPUT%\data
mkdir %OUTPUT%\engine
mkdir %OUTPUT%\engine\ibus
copy ..\rime\engine\*.py %OUTPUT%\engine
copy ..\rime\weasel\*.py %OUTPUT%\engine
copy ..\rime\weasel\ibus\*.py %OUTPUT%\engine\ibus
copy misc\*.conf %OUTPUT%\engine
copy misc\*.bat %OUTPUT%
copy misc\*.js %OUTPUT%
copy misc\*.py %OUTPUT%
copy misc\*.reg %OUTPUT%
copy misc\*.txt %OUTPUT%
copy Release\WeaselServer.exe %OUTPUT%
copy Release\weasel.ime %OUTPUT%\weasels.ime
copy ReleaseHant\weasel.ime %OUTPUT%\weaselt.ime

pause
