@call ..\env.bat
@echo installing schema Kuankhiunn.

..\WeaselServer.exe /q
python create-schema.py -n -v Kuankhiunn.txt

@pause
