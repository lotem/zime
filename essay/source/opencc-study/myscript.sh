# format: source \t length \t hant \t hans \t reading (null value here)
awk '
{
    s = $1
    for (i = 2; i <= NF; ++i)
        print "opencc\t" length(s) "\t" $i "\t" s "\t"
}
' char_simp2trad > opencc_char.1
awk '
{
    t = $1
    for (i = 2; i <= NF; ++i)
        print "opencc\t" length(t) "\t" t "\t" $i "\t"
}
' char_trad2simp > opencc_char.2
sort -u opencc_char.[12] > opencc_char
awk '
{
    s = $1
    for (i = 2; i <= NF; ++i)
        print "opencc\t" length(s) "\t" $i "\t" s "\t"
}
' phrase_simp2trad > opencc_phrase.1
awk '
{
    t = $1
    for (i = 2; i <= NF; ++i)
        print "opencc\t" length(t) "\t" t "\t" $i "\t"
}
' phrase_trad2simp > opencc_phrase.2
sort -u opencc_phrase.[12] | sort -ns -k2 > opencc_phrase
