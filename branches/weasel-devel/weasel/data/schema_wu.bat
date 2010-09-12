@call ..\env.bat
@echo installing schema Wu.

..\WeaselServer.exe /q
python make-phrases.py wu
type wu-extra-phrases.txt >> wu-phrases.txt
python create-schema.py -v Wu.txt

@pause
