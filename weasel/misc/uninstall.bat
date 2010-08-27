@echo off
call env.bat
call stop_service.bat

if defined OSVersion goto uninstall

set /p Answer="are you using Weasel on Windows XP [Y/n]? "
if /i "%Answer%" == "n" (
    set OSVersion=win7
) else (
    set OSVersion=xp
)
echo OSVersion = %OSVersion%

:uninstall
echo uninstalling Weasel ime.
goto %OSVersion%_uninstall

:win7_uninstall
elevate rundll32 "%CD%\weasel.ime" uninstall
goto exit

:xp_uninstall
rundll32 "%CD%\weasel.ime" uninstall
goto exit

:exit
