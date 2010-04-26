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
import json
import re

def debug(*what):
    print >> sys.stderr, '[DEBUG]: ', ' '.join(map(unicode, what))

class SpellingCollisionError:
    def __init__(self, rule, vars):
        self.rule = rule
        self.vars = vars
    def __str__(self):
        return 'spelling collision detected in %s: %s' % (self.rule, repr(self.vars))

class SpellingAlgebra:

    def __init__(self, report_errors=True):
        self.__report_errors = report_errors

    def calculate(self, mapping_rules, fuzzy_rules, spelling_rules, alternative_rules, keywords):

        akas = dict()

        def add_aka(s, x):
            if s in akas:
                a = akas[s]
            else:
                a = akas[s] = []
            if x not in a:
                a.append(x)

        def del_aka(s, x):
            if s in akas:
                a = akas[s]
            else:
                a = akas[s] = []
            if x in a:
                a.remove(x)
            if not a:
                del akas[s]

        def transform(x, r):
            return r[0].sub(r[1], x, 1)

        def apply_fuzzy_rule(d, r):
            dd = dict(d)
            for x in d:
                if not r[0].search(x):
                    continue
                y = transform(x, r)
                if y == x:
                    continue
                if y not in dd:
                    dd[y] = d[x]
                    add_aka(dd[y], y)
                else:
                    del_aka(dd[y], y)
                    dd[y] |= d[x]
                    add_aka(dd[y], y)
            return dd

        def apply_alternative_rule(d, r):
            for x in d.keys():
                if not r[0].search(x):
                    continue
                y = transform(x, r)
                if y == x:
                    continue
                if y not in d:
                    d[y] = d[x]
                elif self.__report_errors:
                    raise SpellingCollisionError('AlternativeRule', (x, d[x], y, d[y]))
            return d

        io_map = dict()
        for okey in keywords:
            ikey = reduce(transform, mapping_rules, okey)
            s = frozenset([okey])
            if ikey in io_map:
                io_map[ikey] |= s
            else:
                io_map[ikey] = s
        for ikey in io_map:
            add_aka(io_map[ikey], ikey)
        io_map = reduce(apply_fuzzy_rule, fuzzy_rules, io_map)

        oi_map = dict()
        ikeys = []
        spellings = []
        for okeys in akas:
            ikey = akas[okeys][0]
            ikeys.append(ikey)
            for x in akas[okeys]:
                spellings.append((x, ikey))
            for k in okeys:
                if k in oi_map:
                    a = oi_map[k]
                else:
                    a = oi_map[k] = []
                a.append(ikey)
        akas = None

        # remove non-ikey keys
        io_map = dict([(k, list(io_map[k])) for k in ikeys])

        spelling_map = dict()
        for s, ikey in spellings:
            t = reduce(transform, spelling_rules, s)
            if t not in spelling_map:
                spelling_map[t] = ikey
            elif self.__report_errors:
                raise SpellingCollisionError('SpellingRule', (s, ikey, t, spelling_map[t]))
        spelling_map = reduce(apply_alternative_rule, alternative_rules, spelling_map)

        return spelling_map, io_map, oi_map

usage = 'usage: %prog [options] YourSchema.txt'
parser = optparse.OptionParser(usage)

parser.add_option('-d', '--dir', dest='dir', default='..', help='target plume.js root dir')
parser.add_option('-k', '--keep', action='store_true', dest='keep', default=False, help='keep existing dict')
parser.add_option('-l', '--level', type='int', dest='level', default=1, help='level of indexing')
parser.add_option('-p', '--pretty', action='store_true', dest='pretty', default=False, help='pretty json output')
parser.add_option('-n', '--no-phrases', action='store_true', dest='no_phrases', default=False, help='do not use phrase file')
parser.add_option('-s', '--source', dest='source', help='specify the prefix of source dict files', metavar='PREFIX')
parser.add_option('-v', '--verbose', action='store_true', dest='verbose', default=False, help='make lots of noice')

options, args = parser.parse_args()

if len(args) != 1:
    parser.error('incorrect number of arguments')
schema_file = args[0] if len(args) > 0 else None

LIMIT = 512

dest_dir = os.path.join(options.dir, 'json')

schema = None
display_name = None
dict_prefix = None

max_key_length = 2

mapping_rules = []
fuzzy_rules = []
spelling_rules = []
alternative_rules = []

config = []

if schema_file:
    equal_sign = re.compile(ur'\s*=\s*')
    compile_repl_pattern = lambda x: (re.compile(x[0]), x[1])
    back_ref = re.compile(ur'\\(\d+)')
    back_ref_g = re.compile(ur'\\g<(\d+)>')
    def to_js_regex(r):
        p = r.split(None, 1)
        if len(p) < 2:
            return r
        p[1] = back_ref.sub(ur'$\1', back_ref_g.sub(ur'$\1', p[1]))
        return u' '.join(p)
    f = open(schema_file, 'r')
    for line in f:
        x = line.strip().decode('utf-8')
        if not x or x.startswith('#'):
            continue
        try:
            (path, value) = equal_sign.split(x, 1)
        except:
            print >> sys.stderr, 'error parsing (%s) %s' % (schema_file, x)
            exit()
        if not schema and path == u'Schema':
            schema = value
            print >> sys.stderr, 'schema: %s' % schema
        if schema:
            if path == u'DisplayName':
                display_name = value
            if not dict_prefix and path == u'Dict':
                dict_prefix = value
                print >> sys.stderr, 'dict: %s' % dict_prefix
            if path == u'MaxKeyLength':
                max_key_length = int(value)
            elif path == u'MappingRule':
                mapping_rules.append(compile_repl_pattern(value.split()))
            elif path == u'FuzzyRule':
                fuzzy_rules.append(compile_repl_pattern(value.split()))
            elif path == u'SpellingRule':
                spelling_rules.append(compile_repl_pattern(value.split()))
            elif path == u'AlternativeRule':
                alternative_rules.append(compile_repl_pattern(value.split()))
        if path.endswith(u'Rule'):
            value = to_js_regex(value) 
        config.append((path, value))
    f.close()

if not dict_prefix:
    print >> sys.stderr, 'no dict specified in schema file.'
    exit()

prefix_args = {'prefix' : dict_prefix}

source_file_prefix = options.source or dict_prefix.replace(u'_', u'-')
keyword_file = '%s-keywords.txt' % source_file_prefix
phrase_file = '%s-phrases.txt' % source_file_prefix if not options.no_phrases else None

keywords = dict()
if keyword_file:
    f = open(keyword_file, 'r')
    for line in f:
        x = line.strip().decode('utf-8')
        if not x or x.startswith(u'#'):
            continue
        try:
            ll = x.split(u'\t', 1)
            (okey, phrase) = ll
        except:
            print >> sys.stderr, 'error: invalid format (%s) %s' % (keyword_file, x)
            exit()
        if okey not in keywords:
            keywords[okey] = [phrase]
        else:
            keywords[okey].append(phrase)
    f.close()

sa = SpellingAlgebra()
try:
    spelling_map, io_map, oi_map = sa.calculate(mapping_rules, 
                                                fuzzy_rules, 
                                                spelling_rules, 
                                                alternative_rules, 
                                                keywords)
except SpellingCollisionError as e:
    print >> sys.stderr, e
    exit()

def rm_R(top):
    for root, dirs, files in os.walk(top, topdown=False):
        for name in files:
            os.remove(os.path.join(root, name))
        for name in dirs:
            os.rmdir(os.path.join(root, name))

def mkdir_p(dir):
    if not os.path.exists(dir):
        os.makedirs(dir)

def dump_schema_config():
    output_config_file = os.path.join(dest_dir, '%sConfig.json' % schema)
    data = {'config': config, 'spellingMap': spelling_map, 'ioMap': io_map, 'oiMap': oi_map}
    json.dump(data, open(output_config_file, 'wb'), indent=(2 if options.pretty else None))

def update_schema_list():
    global schema, display_name
    schema_list_file = os.path.join(dest_dir, 'SchemaList.json')
    if os.path.exists(schema_list_file):
        data = filter(lambda x: x['schema'] != schema, json.load(open(schema_list_file, 'rb')))
    else:
        data = []
    s = {'schema': schema, 'displayName': display_name}
    data.append(s)
    json.dump(data, open(schema_list_file, 'wb'), indent=(2 if options.pretty else None))

mkdir_p(dest_dir)
dump_schema_config()
update_schema_list()

if options.keep:
    print >> sys.stderr, 'done.'
    exit()

def g(ikeys, okey, depth):
    if not okey or depth >= max_key_length:
        return ikeys
    r = []
    for x in ikeys:
        if okey[0] not in oi_map:
            if options.verbose:
                print >> sys.stderr, 'invalid keyword encountered: [%s]' % okey[0]
            return []
        for y in oi_map[okey[0]]:
            r.append(x + [y])
    return g(r, okey[1:], depth + 1)

phrase_counter = 0
phrases = dict()
freq_total = 0
ip_map = dict()

def process_phrase(okey, phrase, freq):
    global phrase_counter, phrases, freq_total, ip_map
    phrase_counter += 1
    freq_total += freq
    k = (okey, phrase)
    if k in phrases:
        phrases[k] += freq
    else:
        phrases[k] = freq
        ikeys = g([[]], okey.split(), 0)
        if not ikeys and options.verbose:
            print >> sys.stderr, 'failed index generation for phrase [%s] %s.' % (okey, phrase)
        for i in ikeys:
            ikey = u' '.join(i)
            if ikey in ip_map:
                ip_map[ikey].add(k)
            else:
                ip_map[ikey] = set([k])

for k in keywords:
    for p in keywords[k]:
        process_phrase(k, p[1:] if p.startswith(u'*') else p, 0)
        if options.verbose and phrase_counter % 1000 == 0:
            print >> sys.stderr, '%dk phrases imported from %s.' % (phrase_counter / 1000, keyword_file)
del keywords

if phrase_file:
    f = open(phrase_file, 'r')
    for line in f:
        x = line.strip().decode('utf-8')
        if not x or x.startswith(u'#'):
            continue
        try:
            ll = x.split(u'\t', 2)
            if len(ll) == 3:
                (phrase, freq_str, okey) = ll
                freq = int(freq_str)
            else:
                (okey, phrase) = ll
                if phrase.startswith(u'*'):
                    phrase = phrase[1:]
                freq = 0
            if u' ' in phrase:
                phrase = phrase.replace(u' ', '')
        except:
            print >> sys.stderr, 'error: invalid format (%s) %s' % (phrase_file, x)
            exit()
        process_phrase(okey, phrase, freq)
        if options.verbose and phrase_counter % 1000 == 0:
            print >> sys.stderr, '%dk phrases imported from %s.' % (phrase_counter / 1000, phrase_file)
    f.close()

rm_R(os.path.join(dest_dir, dict_prefix))
mkdir_p(os.path.join(dest_dir, dict_prefix))

def get_index(ikey):
    return u'_'.join(ikey.split(u' ')[:options.level])

files = {}
indexed_phrases = {}
indexed_phrase_count = 0

def dump_dict_index(index):
    file_path = os.path.join(dest_dir, dict_prefix, '%s.json' % files[index])
    f = open(file_path, 'wb')
    json.dump(indexed_phrases, f, indent=(2 if options.pretty else None))
    if options.verbose:
        print >> sys.stderr, '%d phrases of index %s written to %s.' % (indexed_phrase_count, index, file_path)

current = None

for ikey in sorted(ip_map):
    index = get_index(ikey)
    if index != current:
        if current:
            dump_dict_index(current)
        current = index
        indexed_phrase_count = 0
        indexed_phrases = {}
        files[index] = index.encode('utf-7')
    if ikey in indexed_phrases:
        s = indexed_phrases[ikey]
    else:
        s = indexed_phrases[ikey] = []
    for k in ip_map[ikey]:
        freq = phrases[k]
        s.append((k[0], k[1], freq))
        indexed_phrase_count += 1
    s.sort(cmp=lambda a, b: -cmp(a[2], b[2]))
    if len(s) > LIMIT:
        del s[LIMIT:]

if current:
    dump_dict_index(current)

if options.verbose:
    print >> sys.stderr, 'totaling %d files.' % len(files)

def dump_dict_info():
    data = {'freqTotal': freq_total, 'indexingLevel': options.level, 'files': files}
    output_dict_file = os.path.join(dest_dir, '%s.json' % dict_prefix)
    json.dump(data, open(output_dict_file, 'wb'), indent=(2 if options.pretty else None))

dump_dict_info()

print >> sys.stderr, 'done.'

