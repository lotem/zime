@echo off
call env.bat
call stop_service.bat

cd data

:menu
echo schema available:
echo --------------------------------------------------
echo Pinyin      - simplified :(
echo TonalPinyin - recommended :)
echo Zhuyin      - bopomofo
echo Jyutping    - cantonese
echo Wu          - zanhe rerau
echo --------------------------------------------------
set Choise=
set /p Choise="what's the schema of your choice [Ptzjw]? "
if /i "%Choise%" == "" goto pinyin
if /i "%Choise%" == "p" goto pinyin
if /i "%Choise%" == "t" goto tonal_pinyin
if /i "%Choise%" == "z" goto zhuyin
if /i "%Choise%" == "j" goto jyutping
if /i "%Choise%" == "w" goto wu

echo "%Choise%"... that isn't what we offered.
goto menu

:pinyin
echo installing schema Pinyin.
python create-schema.py -v Pinyin.txt
python create-schema.py -v -k DoublePinyin.txt
python create-schema.py -v -k ComboPinyin.txt
goto exit

:tonal_pinyin
echo installing schema TonalPinyin.
python make-phrases.py tonal-pinyin
python create-schema.py -v TonalPinyin.txt
goto exit

:zhuyin
echo installing schema Zhuyin.
python make-phrases.py zhuyin
python create-schema.py -v Zhuyin.txt
goto exit

:jyutping
echo installing schema Jyutping.
python make-phrases.py jyutping
python create-schema.py -v Jyutping.txt
goto exit

:wu
echo installing schema Wu.
python make-phrases.py wu
type wu-extra-phrases.txt >> wu-phrases.txt
python create-schema.py -v Wu.txt

:exit
cd ..
pause
