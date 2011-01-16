set OUTPUT=output\combopinyin
rmdir /s /q %OUTPUT%
mkdir %OUTPUT%
mkdir %OUTPUT%\config
copy config\*.txt %OUTPUT%\config
mkdir %OUTPUT%\doc
copy doc\*.pdf %OUTPUT%\doc
copy combopinyin.exe %OUTPUT%
copy *.png %OUTPUT%
copy *.txt %OUTPUT%

pause
