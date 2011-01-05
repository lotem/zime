#! /bin/bash
sort -s -k3 vocabulary | sort -ns -k2 |
awk '
BEGIN { id = 0; last = "" }
{
    if (last == $3)
        print "INFO: repeated", $3 > "/dev/stderr"
    else
    {
        print id "\t" $3
        ++id
    }
    last = $3
}
' > phraseid 2> log
