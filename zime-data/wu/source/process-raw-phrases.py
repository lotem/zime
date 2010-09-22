#!/usr/bin/env python
# encoding: utf-8
import sys

if len(sys.argv) < 2:
    print >> sys.stderr, "usage: %s dict" % sys.argv[0]
    exit()

kw = dict()
for line in open("%s-keywords.txt" % sys.argv[1]):
    line = line.strip().decode('utf-8')
    if not line or line.startswith(u'#'):
        continue
    t = line.split(u'\t')
    if len(t) < 2:
        print >> sys.stderr, "error parsing:", line
        exit(1)
    key, word = t
    if word.startswith(u'*'):
        word = word[1:]
    if word in kw:
        kw[word].add(key)
    else:
        kw[word] = set([key])

freq_table = dict()
for line in sys.stdin:
    line = line.strip().decode('utf-8')
    if not line or line.startswith(u'#'):
        continue
    t = line.split()
    if len(t) < 2:
        print >> sys.stderr, "error parsing:", line
        continue
    spelling, phrase = t
    syllables = []
    pos = 0
    for word in phrase:
        if word not in kw:
            print >> sys.stderr, ("[%s]: unrecognized char in %s" % (word, phrase)).encode('utf-8')
            break
        match = None
        for key in kw[word]:
            if spelling.startswith(key, pos):
                if not match or match < key:
                    match = key
        if not match:
            print >> sys.stderr, ("[%s] %s" % (word, u' '.join(list(kw[word])))).encode('utf-8')
            break
        syllables.append(match)
        pos += len(match)
    if pos < len(spelling):
        print >> sys.stderr, ("mismatch: %s %s|%s" % (phrase, spelling[:pos], spelling[pos:])).encode('utf-8')
        print >> sys.stderr
        continue
    key = u' '.join(syllables)
    if key in freq_table:
        freq = max(freq_table[key] - 1, 1)
        freq_table[key] = freq
    else:
        freq = freq_table[key] = 100
    print ("%s\t%d\t%s" % (phrase, freq, key)).encode('utf-8')

