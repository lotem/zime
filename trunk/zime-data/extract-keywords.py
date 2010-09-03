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

phrase_file = open('%s-phrases.txt' % prefix)

for line in phrase_file:
    x = line.strip().decode('utf-8')
    if not x or x.startswith(u'#'):
        continue
    phrase, freq_str, key = x.split(u'\t', 2)
    try:
        freq = int(freq_str)
    except:
        print >> sys.stderr, u'error: invalid format (%s) %s' % (x, options.source_file)
        exit()
    # split phrases into single words
    # specially, if the phrase contain multi-character words, space should be used to delimit EACH word in that phrase
    if u' ' in phrase:
        words = phrase.split()
    else:
        words = list(phrase)
    keywords = key.split()
    if len(words) != len(keywords):
        print >> sys.stderr, u'warning: syllables mismatch [%s] %s' % (phrase, key)

phrase_file.close()

