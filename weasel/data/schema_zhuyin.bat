@call ..\env.bat
@echo installing schema Zhuyin.

..\WeaselServer.exe /q
python make-phrases.py zhuyin
python create-schema.py -v Zhuyin.txt

@pause
