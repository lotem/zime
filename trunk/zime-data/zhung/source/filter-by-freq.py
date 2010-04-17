#! /usr/bin/env python

try:
    import psyco
    psyco.full()
    print 'psyco activated.'
except:
    pass

word_map = dict()
for line in open('phrases.txt'):
    x = line.strip().decode('utf-8')
    if not x or x.startswith(u'#'):
        continue
    phrase, freq_str = x.split(None, 1)
    if len(phrase) > 1:
        continue
    try:
        freq = int(freq_str)
    except:
        print >> sys.stderr, 'error: invalid format (%s) %s' % (x, 'phrases.txt')
    word_map[phrase] = [freq, []]

first = True
for line in open('zz-all.tsv'):
    if first:
        print line,
        first = False
        continue
    x = line.rstrip('\n').decode('utf-8')
    if not x or x.startswith(u'#'):
        continue
    a = x.split(u'\t')
    if a[1] in word_map:
        word_map[a[1]][1].append(a)

for x in sorted(word_map.values(), key=lambda p: p[0], reverse=True):
    for a in x[1]:
        print u'\t'.join(a + [str(x[0])]).encode('utf-8')
