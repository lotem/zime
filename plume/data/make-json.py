#! /usr/bin/env python

try:
    import psyco
    psyco.full ()
    print 'psyco activated.'
except:
    pass

import os
import sys
import optparse
import json
import re

def debug (*what):
    print >> sys.stderr, '[DEBUG]: ', ' '.join (map (unicode, what))

usage = 'usage: %prog [options] schema-file [keyword-file [phrase-file]]'
parser = optparse.OptionParser (usage)

parser.add_option ('-s', '--schema', dest='schema', help='shortcut to specifying a standard set of input file names')
parser.add_option ('-k', '--keep', action='store_true', dest='keep', default=False, help='keep existing dict')
parser.add_option ('-l', '--level', type='int', dest='level', default=1, help='level of indexing')
parser.add_option ('-p', '--pretty', action='store_true', dest='pretty', default=False, help='pretty json output')

parser.add_option ('-v', '--verbose', action='store_true', dest='verbose', default=False, help='make lots of noice')

options, args = parser.parse_args ()

if options.schema:
    schema_file = '%s-schema.txt' % options.schema
    keyword_file = '%s-keywords.txt' % options.schema
    phrase_file = '%s-phrases.txt' % options.schema
else:
    if len (args) not in range (1, 4):
        parser.error ('incorrect number of arguments')
    schema_file = args[0] if len (args) > 0 else None
    keyword_file = args[1] if len (args) > 1 else None
    phrase_file = args[2] if len (args) > 2 else None

DEST_DIR = os.path.join ('..', 'war', 'script', 'data')
LIMIT = 512

schema = None
schema_name = None
prefix = None
delim = None
max_key_length = 3

config = []
spelling_rules = []
fuzzy_rules = []

if schema_file:
    equal_sign = re.compile (ur'\s*=\s*')
    compile_repl_pattern = lambda x: (re.compile (x[0]), x[1])
    f = open (schema_file, 'r')
    for line in f:
        x = line.strip ()
        if not x or x.startswith ('#'):
            continue
        try:
            (path, value) = equal_sign.split (x, 1)
        except:
            print >> sys.stderr, 'error parsing (%s) %s' % (schema_file, x)
            exit ()
        if not schema:
            m = re.match (ur'Schema/(\w+)', path)
            if m:
                schema = m.group (1)
                schema_name = value
                print >> sys.stderr, 'processing schema: %s' % schema
        else:
            if not prefix and path == u'Config/%s/Prefix' % schema:
                prefix = value
                print >> sys.stderr, 'dict prefix: %s' % prefix
            #if not delim and path == u'Config/%s/Delimiter' % schema:
            #    if value[0] == u'[' and value[-1] == u']':
            #        delim = value[1]
            #    else:
            #        delim = value[0]
            if path == u'Config/%s/MaxKeyLength' % schema:
                max_key_length = int (value)
            elif path == u'Config/%s/SpellingRule' % schema:
                spelling_rules.append (compile_repl_pattern (value.split ()))
            elif path == u'Config/%s/FuzzyRule' % schema:
                fuzzy_rules.append (compile_repl_pattern (value.split ()))
        config.append ((path, value))
    f.close ()

if not prefix:
    print >> sys.stderr, 'no dict prefix specified in schema file.'
    exit ()

phrase_counter = 0
phrases = dict ()

okeys = set ()

if keyword_file:
    f = open (keyword_file, 'r')
    for line in f:
        x = line.strip ().decode ('utf-8')
        if not x or x.startswith (u'#'):
            continue
        try:
            ll = x.split (u'\t', 1)
            (okey, phrase) = ll
        except:
            print >> sys.stderr, 'error: invalid format (%s) %s' % (phrase_file, x)
            exit ()
        if okey not in okeys:
            okeys.add (okey)
        phrases[(okey, phrase)] = 0
        phrase_counter += 1
        if options.verbose and phrase_counter % 1000 == 0:
            print >> sys.stderr, '%dk phrases imported from %s.' % (phrase_counter / 1000, keyword_file)
    f.close ()

def apply_spelling_rule (m, r):
    return (r[0].sub (r[1], m[0], 1), m[1])
d = dict ([reduce (apply_spelling_rule, spelling_rules, (k, frozenset ([k]))) for k in okeys])
akas = dict ()
def add_aka (s, x):
    if s in akas:
        a = akas[s]
    else:
        a = akas[s] = []
    if x not in a:
        a.append (x)
def del_aka (s, x):
    if s in akas:
        a = akas[s]
    else:
        a = akas[s] = []
    if x in a:
        a.remove (x)
    if not a:
        del akas[s]
def apply_fuzzy_rule (d, r):
    dd = dict (d)
    for x in d:
        if not r[0].search (x):
            continue
        y = r[0].sub (r[1], x, 1)
        if y == x:
            continue
        if y not in dd:
            dd[y] = d[x]
            add_aka (dd[y], y)
        else:
            del_aka (dd[y], y)
            dd[y] |= d[x]
            add_aka (dd[y], y)
    return dd
for k in d:
    add_aka (d[k], k)
fuzzy_map = dict ([(k, list (v)) for k, v in reduce (apply_fuzzy_rule, fuzzy_rules, d).iteritems ()])
keywords = dict ()
oi_map = dict ()
for s in akas:
    spelling = akas[s][0]
    for x in akas[s]:
        keywords[x] = spelling
    for k in s:
        if k in oi_map:
            a = oi_map[k]
        else:
            a = oi_map[k] = []
        a.append (spelling)
del akas

output_config_file = os.path.join (DEST_DIR, '%sConfig.json' % schema)
data = {'fuzzyMap': fuzzy_map, 'keywords': keywords, 'config': config}
json.dump (data, open (output_config_file, 'wb'))

def update_schema_list ():
    global schema, schema_name
    schema_list_file = os.path.join (DEST_DIR, 'SchemaList.json')
    if os.path.exists (schema_list_file):
        data = filter (lambda x: x['schema'] != schema, json.load (open (schema_list_file, 'rb')))
    else:
        data = []
    s = {'schema': schema, 'displayName': schema_name}
    data.append (s)
    json.dump (data, open (schema_list_file, 'wb'))

if options.keep:
    update_schema_list ()
    print >> sys.stderr, 'done.'
    exit ()

def g (s, k, depth):
    if not k or depth >= max_key_length:
        return s
    r = []
    for x in s:
        if k[0] not in oi_map:
            if options.verbose:
                print >> sys.stderr, 'invalid keyword encountered: [%s]' % k[0]
            return []
        for y in oi_map[k[0]]:
            r.append (x + [y])
    return g(r, k[1:], depth + 1)

freq_total = 0
ip_map = dict ()

def process_phrase (okey, phrase, freq):
    global phrase_counter, phrases, freq_total, i_map
    phrase_counter += 1
    k = (okey, phrase)
    if k in phrases:
        phrases[k] += freq
    else:
        phrases[k] = freq
    freq_total += freq
    ikeys = g ([[]], okey.split (), 0)
    if not ikeys and options.verbose:
        print >> sys.stderr, 'failed index generation for phrase [%s] %s.' % (okey, phrase)
    for i in ikeys:
        ikey = u' '.join (i)
        if ikey in ip_map:
            ip_map[ikey].add (k)
        else:
            ip_map[ikey] = set ([k])

if phrase_file:
    f = open (phrase_file, 'r')
    for line in f:
        x = line.strip ().decode ('utf-8')
        if not x or x.startswith (u'#'):
            continue
        try:
            ll = x.split (u'\t', 2)
            if len (ll) == 3:
                (phrase, freq_str, okey) = ll
                freq = int (freq_str)
            else:
                (okey, phrase) = ll
                if phrase.startswith (u'*'):
                    phrase = phrase[1:]
                freq = 0
        except:
            print >> sys.stderr, 'error: invalid format (%s) %s' % (phrase_file, x)
            exit ()
        process_phrase (okey, phrase, freq)
        if options.verbose and phrase_counter % 1000 == 0:
            print >> sys.stderr, '%dk phrases imported from %s.' % (phrase_counter / 1000, phrase_file)
    f.close ()

def rm_R (top):
    for root, dirs, files in os.walk (top, topdown=False):
        for name in files:
            os.remove (os.path.join (root, name))
        for name in dirs:
            os.rmdir (os.path.join (root, name))

def mkdir_p (dir):
    if not os.path.exists (dir):
        os.makedirs (dir)

rm_R (os.path.join (DEST_DIR, prefix))
mkdir_p (os.path.join (DEST_DIR, prefix))

def get_idx (ikey):
    return u'_'.join (ikey.split (u' ')[:options.level])

current = None
sum = 0
ph = {}
indices = []

def dump_json (key):
    file_path = os.path.join (DEST_DIR, prefix, '%s.json' % key)
    f = open (file_path, 'wb')
    json.dump (ph, f, indent=(2 if options.pretty else None))
    if options.verbose:
        print >> sys.stderr, '%d phrases written to %s.' % (sum, file_path)

for ikey in sorted (ip_map):
    idx = get_idx (ikey)
    if idx != current:
        if current:
            dump_json (current)
        current = idx
        sum = 0
        ph = {}
        indices.append (idx)
    if ikey in ph:
        s = ph[ikey]
    else:
        s = ph[ikey] = []
    for k in ip_map[ikey]:
        freq = phrases[k]
        s.append ((k[0], k[1], freq))
        sum += 1
    s.sort (cmp=lambda a, b: -cmp (a[2], b[2]))
    if len (s) > LIMIT:
        del s[LIMIT:]

if current:
    dump_json (current)

if options.verbose:
    print >> sys.stderr, 'totaling %d indices.' % len (indices)

data = {'freqTotal': freq_total, 'indexingLevel': options.level, 'indices': indices}
output_dict_file = os.path.join (DEST_DIR, '%s.json' % prefix)
json.dump (data, open (output_dict_file, 'wb'))

update_schema_list ()
print >> sys.stderr, 'done.'

