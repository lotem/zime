#! /usr/bin/env python

try:
    import psyco
    psyco.full()
    print 'psyco activated.'
except:
    pass

import os
import sys
import optparse

def debug(*what):
    print >> sys.stderr, u'[DEBUG]: ', u' '.join(map(unicode, what))

usage = 'usage: %prog [options] dictname'
parser = optparse.OptionParser(usage)
parser.add_option('-r', '--ratio', action='store', type='int', dest='ratio', default=10, help='ratio (in %) below which the keyword is considered rare.')
parser.add_option('-v', '--verbose', action='store_true', dest='verbose', default=False, help='make lots of noice.')
options, args = parser.parse_args()

if len(args) < 1:
    parser.error('missing dictname')
prefix = args[0]


word_map = dict()

def scan(input_file):
    for line in input_file:
        x = line.strip().decode('utf-8')
        if not x or x.startswith(u'#'):
            continue
        # parse line
        try:
            ll = x.split(u'\t', 2)
            if len(ll) == 3:
                # phrase file
                (phrase, freq_str, key) = ll
                # count all occurences of keywords; no zeros
                freq = int(freq_str) + 1
            else:
                # keyword file
                (key, phrase) = ll
                freq = 0
                if phrase.startswith(u'*'):
                    # explicitly marked rare
                    phrase = phrase[1:]
                    freq = -1
        except:
            print >> sys.stderr, 'error: invalid format [%s]' % x
            exit()
        # split phrases into single words
        # specially, if the phrase contain multi-character words, space should be used to delimit EACH word in that phrase
        if u' ' in phrase:
            words = phrase.split()
        else:
            words = list(phrase)
        keywords = key.split()
        if len(words) != len(keywords):
            print >> sys.stderr, 'warning: syllables mismatch [%s] %s' % (phrase, key)
        # calculate keyword frequency
        for w, kw in zip(words, keywords):
            if w in word_map:
                kw_freq = word_map[w]
            else:
                kw_freq = word_map[w] = dict()
            if kw in kw_freq:
                if kw_freq[kw] >= 0:
                    kw_freq[kw] += freq
            else:
                kw_freq[kw] = freq

keyword_file = open('%s-keywords.txt' % prefix)
scan(keyword_file)
keyword_file.close()

phrase_file = open('%s-phrases.txt' % prefix)
scan(phrase_file)
phrase_file.close()


# output keywords

out = list()

for w in word_map:
    kw_freq = word_map[w]
    total_freq = sum(kw_freq.values())
    for kw, freq in kw_freq.iteritems():
        is_rare = freq < total_freq * options.ratio / 100
        fmt = u'%s\t*%s' if is_rare else u'%s\t%s'
        out.append(fmt % (kw, w))

output_file = open('%s-keywords.out' % prefix, 'w')
for x in sorted(out):
    print >> output_file, x.encode('utf-8')
output_file.close()

print '%s-keywords.out written.' % prefix
