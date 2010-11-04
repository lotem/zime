#! /bin/bash

awk '
BEGIN { id = 0 }
{
    ++id
    print id "\t" $3 "\t" $4
}
' vocabulary > phrase_id
