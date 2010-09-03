@echo off
call env.bat
call stop_service.bat

echo uninstalling Weasel ime.
echo OSVersion = %OSVersion%
if defined OSVersion goto %OSVersion%_uninstall

ver | python checkosver.py
if %ERRORLEVEL% EQU 5 goto xp_uninstall

:win7_uninstall
elevate rundll32 "%CD%\weasel.ime" uninstall
goto exit

:xp_uninstall
rundll32 "%CD%\weasel.ime" uninstall
goto exit

:exit
