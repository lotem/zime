#! /bin/bash

#sort cedict iii | uniq -d > common_cedict_iii
#sort cedict common_cedict_iii | uniq -u | awk '{print "cedict\t" $0}' > uniq_cedict_cmp_iii
#sort iii common_cedict_iii | uniq -u | awk '{print "iii\t" $0}' > uniq_iii_cmp_cedict
#sort -k2 -k4 -k3 -k1 uniq_cedict_cmp_iii uniq_iii_cmp_cedict | uniq -f3 --all-repeated=separate > diff_trad_cedict_iii

#sort chewing iii | uniq -d > common_chewing_iii
#sort iii common_chewing_iii | uniq -u | awk '{print "iii\t" $0}' > uniq_iii_cmp_chewing
#sort chewing common_chewing_iii | uniq -u | awk '{print "chewing\t" $0}' > uniq_chewing_cmp_iii
#sort -k2 -k4 -k3 -k1 uniq_iii_cmp_chewing uniq_chewing_cmp_iii | uniq -f3 --all-repeated=separate > diff_trad_chewing_iii

awk '{print "opencc\t" $0}' opencc > opencc_with_tag

sort opencc cedict | uniq -d > common_opencc_cedict
sort cedict common_opencc_cedict | uniq -u | awk '{print "cedict\t" $0}' > uniq_cedict_cmp_opencc
sort -k2 -k4 -k3 -k1 uniq_cedict_cmp_opencc opencc_with_tag | uniq -f3 --all-repeated=separate > diff_trad_cedict_opencc

sort opencc chewing | uniq -d > common_opencc_chewing
sort chewing common_opencc_chewing | uniq -u | awk '{print "chewing\t" $0}' > uniq_chewing_cmp_opencc
sort -k2 -k4 -k3 -k1 uniq_chewing_cmp_opencc opencc_with_tag | uniq -f3 --all-repeated=separate > diff_trad_chewing_opencc

sort opencc iii | uniq -d > common_opencc_iii
sort iii common_opencc_iii | uniq -u | awk '{print "iii\t" $0}' > uniq_iii_cmp_opencc
sort -k2 -k4 -k3 -k1 uniq_iii_cmp_opencc opencc_with_tag | uniq -f3 --all-repeated=separate > diff_trad_iii_opencc
