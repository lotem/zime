@echo off

set IME_FILE=weasels.ime
if /i "%1" == "/t" set IME_FILE=weaselt.ime

start notepad README.txt

check_python
call env.bat

python --version 2> nul
if %ERRORLEVEL% EQU 0 goto python_ok

if exist .\python-2.7.msi goto install_python
if exist ..\python-2.7.msi goto install_python_pdir

echo error: cannot locate Python installer.
pause
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
call install_schema.bat

:db_ok

echo registering Weasel ime.

ver | python check_osver.py
if %ERRORLEVEL% EQU 5 goto xp_install

:win7_install
elevate rundll32 "%CD%\%IME_FILE%" install
goto exit

:xp_install
rundll32 "%CD%\%IME_FILE%" install
goto exit

:exit
