@call ..\env.bat
@echo installing schema Jyutping.

..\WeaselServer.exe /q
python make-phrases.py jyutping
python create-schema.py -v Jyutping.txt

@pause
