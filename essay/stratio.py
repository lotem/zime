#! /usr/bin/env python -w
# coding: utf-8

# 繁－简对应表
ts_map = {}
# 简－繁对应表
st_map = {}
for line in open("vocabulary"):
    t = line.rstrip().split("\t")
    #if t[1] == "1":  # 单字
    hant = t[2]
    hans = t[3]
    if hant in ts_map:
        ts_map[hant].add(hans)
    else:
        ts_map[hant] = set([hans])
    if hans in st_map:
        st_map[hans].add(hant)
    else:
        st_map[hans] = set([hant])
# 只保留一简对多繁字
st_map = dict([(k, v) for (k, v) in st_map.iteritems() if len(v) > 1])

freq_table = {}
for line in open("source/zhuyin-phrases.txt"):
    t = line.rstrip().split("\t")
    hant = t[0]
    if hant not in ts_map:
        continue
    freq = int(t[1])
    if hant in freq_table:
        freq_table[hant] += freq
    else:
        freq_table[hant] = freq

def get_freq(hant):
    if hant in freq_table:
        return freq_table[hant]
    else:
        return 0

for hans, v in st_map.iteritems():
    a = [(get_freq(hant), hant) for hant in v]
    a.sort(reverse=True)
    s = float(sum([freq for (freq, hant) in a]))
    if s == 0:
        continue
    r = [(hant, round((freq / s) * 1000) / 10) for (freq, hant) in a]
    print "%d\t%s\t%s" % (len(hans) / 3, hans, " ".join(["%s %g%%" % (hant, ratio) for (hant, ratio) in r]))

