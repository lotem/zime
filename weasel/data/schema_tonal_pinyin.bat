@call ..\env.bat
@echo installing schema TonalPinyin.

..\WeaselServer.exe /q
python make-phrases.py tonal-pinyin
python create-schema.py -v TonalPinyin.txt

@pause
