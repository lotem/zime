#! /bin/bash

awk -F "\t" '
{
    source = "chewing"
    reading = $1
    writing = $2
    gsub(/[ *]/, "", writing)
    len = length(writing)
    hant = writing
    hans = ""
    freq = 0
    print source "\t" len "\t" hant "\t" hans "\t" reading
}
' zhuyin-keywords.txt > chewing.1

awk -F "\t" '
{
    source = "chewing"
    reading = $3
    writing = $1
    gsub(/[* ]/, "", writing)
    len = length(writing)
    hant = writing
    hans = ""
    freq = $2
    print source "\t" len "\t" hant "\t" hans "\t" reading
}
' zhuyin-phrases.txt >> chewing.1
sort -u chewing.1 | sort -n -k 2 > chewing
rm chewing.1

awk -F "\t" '
/^#/ { next }
{
    source = "android"
    reading = $3
    writing = $1
    len = length(writing)
    hans = writing
    hant = ""
    freq = $2
    print source "\t" len "\t" hant "\t" hans "\t" reading
}
' android.txt | sort -u | sort -n -k 2 > android

awk -F "\t" '
/^#/ { next }
{
    source = "iii"
    hant = $1
    hans = $2
    len = length(hant)
    reading = ""
    freq = 0
    print source "\t" len "\t" hant "\t" hans "\t" reading
}
' comparison/iii-raw.txt | sort -u | sort -nsk 2 > iii

awk -F "\t" '
/^#/ { next }
{
    source = "cedict"
    hant = $1
    hans = $2
    reading = $3
    len = length(hant)
    freq = 0
    print source "\t" len "\t" hant "\t" hans "\t" reading
}
' comparison/cedict-raw.txt | sort -u | sort -nsk 2 > cedict
sed -i '/集恩廣益/d' cedict

sources="android cedict chewing iii"

cut -f3 $sources | sed '/^$/d' | sort -u > hant.1
cut -f4 $sources | sed '/^$/d' | sort -u > hans.1
opencc -i hant.1 -o hant.s -c /usr/share/opencc/zht2zhs.ini
sort -u hant.s hans.1 > hans.merged

echo '
import fileinput

t = [line.rstrip() for line in open("hant.1")]
s = [line.rstrip() for line in open("hant.s")]
tsmap = dict(zip(t, s))

for line in fileinput.input():
    t = line.rstrip().split("\t")
    if not t[3]:
        if t[2] in tsmap:
            t[3] = tsmap[t[2]]
    print "\t".join(t)

' | python - $sources > merged.1
sort -s -k4 -k1 merged.1 | sort -ns -k2 > merged.2

opencc -i hans.merged -o hant.merged
python > converted.1 <<EOF
hant = [line.rstrip() for line in open("hant.merged")]
hans = [line.rstrip() for line in open("hans.merged")]
for (t, s) in zip(hant, hans):
    print "converted\t%d\t%s\t%s\t" % (len(t.decode("utf-8")), t, s)
EOF
sort -s -k4 converted.1 | sort -ns -k2 > converted

cat opencc-study/opencc_char opencc-study/opencc_phrase > opencc

awk '
BEGIN { OFS = "\t" }
{
    if ($2 == 1)
    {
        # records from opencc come before that of other sources
        if ($1 == "opencc")  
        {
            if ($3 in t)
                t[$3] = t[$3] " " $4
            else
                t[$3] = $4
            $1 = "essay"
        }
        else
        {
            # may be a fake simplified character
            if ($3 in t && index(t[$3], $4) == 0)
                $1 = "!suspicious"
            else
                $1 = "essay"
        }
    }
    else
        $1 = "essay"
    print 
}
' opencc converted > all.1
grep -v '^!suspicious' all.1 | sort -u | sort -s -k4 -k1 | sort -ns -k2 > all
