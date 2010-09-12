@call ..\env.bat
@echo installing schema Pinyin.

..\WeaselServer.exe /q
python create-schema.py -v Pinyin.txt
python create-schema.py -v -k DoublePinyin.txt
python create-schema.py -v -k ComboPinyin.txt

@pause
