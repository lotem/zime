call ..\env.bat
python create-schema.py -v Pinyin.txt
python create-schema.py -v -k DoublePinyin.txt
python create-schema.py -v -k ComboPinyin6.txt
python create-schema.py -v -k ComboPinyin7.txt
python create-schema.py -v TonalPinyin.txt
python make-phrases.py jyutping
python create-schema.py -v Jyutping.txt
pause
