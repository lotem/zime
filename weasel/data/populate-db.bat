call ..\env.bat
python create-schema.py -v Pinyin.txt
python create-schema.py -v -k ComboPinyin.txt
python create-schema.py -v -k DoublePinyin.txt
python make-phrases.py jyutping
python create-schema.py -v Jyutping.txt
pause
