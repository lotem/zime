LC_ALL=zh_TW.UTF-8 awk '{if (length($2) == 1) print $1 "\t" $2}' zaonhe-lopha-raw.txt > wu-lopha-keywords.txt
python process-raw-phrases.py wu-lopha < zaonhe-lopha-raw.txt > wu-lopha-phrases.txt 2> log.txt
