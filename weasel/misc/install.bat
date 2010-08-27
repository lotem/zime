@echo off
call env.bat

python --version 2> nul
if %ERRORLEVEL% EQU 0 goto python_ok

if exist .\python-2.7.msi goto install_python
if exist ..\python-2.7.msi goto install_python_pdir

echo cannot locate Python interpreter.
echo Weasel requires Python 2.7 to be installed first.
goto exit

:install_python
echo running Python installer.
.\python-2.7.msi
goto python_ok

:install_python_pdir
echo running Python installer.
..\python-2.7.msi
goto python_ok

:python_ok

if exist "%UserProfile%\.ibus\zime\zime.db" goto db_ok

echo stopping service.
call stop_service.bat

echo creating ZIME database.
set WeaselRoot=%CD%
cd "%WeaselRoot%\data"
call populate_db.bat
cd "%WeaselRoot%"

:db_ok

if defined OSVersion goto install

set /p Answer="are you using Weasel on Windows XP [Y/n]? "
if /i "%Answer%" == "n" (
    set OSVersion=win7
) else (
    set OSVersion=xp
)
echo OSVersion = %OSVersion%

:install
echo registering Weasel ime.
goto %OSVersion%_install

:win7_install
elevate rundll32 "%CD%\weasel.ime" install
goto exit

:xp_install
rundll32 "%CD%\weasel.ime" install
goto exit

:exit
