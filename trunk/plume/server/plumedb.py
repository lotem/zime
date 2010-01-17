#!/usr/bin/env python

import logging
import os
import re


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
                spelling_map[t] = s
            elif self.__report_errors:
                raise SpellingCollisionError('SpellingRule', (s, ikey, t, spelling_map[t]))
        spelling_map = reduce(apply_alternative_rule, alternative_rules, spelling_map)

        return spelling_map, io_map, oi_map


_schema_pattern = re.compile(r'^[A-Z]\w*$')

_equal_sign = re.compile(ur'\s*=\s*')
_compile_repl_pattern = lambda x: (re.compile(x[0]), x[1])

_back_ref = re.compile(ur'\\(\d+)')
_back_ref_g = re.compile(ur'\\g<(\d+)>')

def _to_js_regex(r):
    p = r.split(None, 1)
    if len(p) < 2:
        return r
    p[1] = _back_ref.sub(ur'$\1', _back_ref_g.sub(ur'$\1', p[1]))
    return u' '.join(p)


class DB:

    LIMIT = 500

    DATA_DIR = 'data'
    SCHEMA_LIST_FILE = 'SchemaList.txt'

    def __init__(self):
        self.__dict = {}
        self.__schema = {}
        self.__load_schema_list()

    def __load_schema_list(self):
        file_path = os.path.join(self.DATA_DIR, self.SCHEMA_LIST_FILE)
        if not os.path.exists(file_path):
            logging.error('missing schema list file.')
            return
        for line in open(file_path):
            schema = line.strip()
            if _schema_pattern.match(schema):
                self.__load_schema(schema)
            
    def __load_schema(self, schema):
        file_path = os.path.join(self.DATA_DIR, '%s.txt' % schema)
        if not os.path.exists(file_path):
            logging.error('missing schema definition file %s.' % file_path)
            return
        schema = None
        display_name = None
        dict_prefix = None
        max_key_length = 2
        mapping_rules = []
        fuzzy_rules = []
        spelling_rules = []
        alternative_rules = []
        config = []
        for line in open(file_path, 'r'):
            x = line.strip().decode('utf-8')
            if not x or x.startswith('#'):
                continue
            try:
                (path, value) = _equal_sign.split(x, 1)
            except:
                print >> sys.stderr, 'error parsing (%s) %s' % (schema_file, x)
                exit()
            if not schema and path == u'Schema':
                schema = value
            if not display_name and path == u'DisplayName':
                display_name = value
            if not dict_prefix and path == u'Dict':
                dict_prefix = value
            if path == u'MaxKeyLength':
                max_key_length = int(value)
            elif path == u'MappingRule':
                mapping_rules.append(_compile_repl_pattern(value.split()))
            elif path == u'FuzzyRule':
                fuzzy_rules.append(_compile_repl_pattern(value.split()))
            elif path == u'SpellingRule':
                spelling_rules.append(_compile_repl_pattern(value.split()))
            elif path == u'AlternativeRule':
                alternative_rules.append(_compile_repl_pattern(value.split()))
            if path.endswith(u'Rule'):
                value = _to_js_regex(value) 
            config.append((path, value))
        if not schema:
            logging.error('no Schema defined in %s.' % file_path)
            return
        if not display_name:
            logging.error('no DisplayName specified for schema %s in %s.' % (schema, file_path))
            return
        if not dict_prefix:
            logging.error('no Dict specified for schema %s in %s.' % (schema, file_path))
            return
        self.__schema[schema] = [display_name, dict_prefix, config, None, 0]
        logging.info('loaded schema %s.' % schema)
        self.__register_dict(dict_prefix, max_key_length, (schema, mapping_rules, fuzzy_rules, spelling_rules, alternative_rules))

    def __register_dict(self, dict_prefix, max_key_length, schema_info):
        if dict_prefix not in self.__dict:
            s = self.__dict[dict_prefix] = [max_key_length, [], None, dict(), -1]
        else:
            s = self.__dict[dict_prefix][1]
        s.append(schema_info)

    def __load_dict(self, dict_prefix):
        if dict_prefix not in self.__dict:
            return
        if self.__dict[dict_prefix][1] == None:
            return
        keyword_file = os.path.join(self.DATA_DIR, '%s-keywords.txt' % dict_prefix)
        if not os.path.exists(keyword_file):
            logging.error('missing keyword file %s.' % keyword_file)
            return
        keywords = dict()
        f = open(keyword_file, 'r')
        for line in f:
            x = line.strip().decode('utf-8')
            if not x or x.startswith(u'#'):
                continue
            try:
                ll = x.split(u'\t', 1)
                (okey, phrase) = ll
            except:
                logging.error('invalid format (%s) %s.' % (keyword_file, x))
                return
            if okey not in keywords:
                keywords[okey] = [phrase]
            else:
                keywords[okey].append(phrase)
        f.close()
        d = self.__dict[dict_prefix]
        sa = SpellingAlgebra()
        first = True
        for s in d[1]:
            try:
                schema, mapping_rules, fuzzy_rules, spelling_rules, alternative_rules = s
                spelling_map, io_map, oi_map = sa.calculate(mapping_rules, 
                                                            fuzzy_rules, 
                                                            spelling_rules, 
                                                            alternative_rules, 
                                                            keywords)
            except SpellingCollisionError, e:
                logging.error('spelling collision error in schema %s: %s' % (schema, e))
                continue
            self.__schema[schema][3] = spelling_map
            if first:
                first = False
                d[2] = (io_map, oi_map)
        d[1] = None
        # TODO: populate dict index
        logging.info('loaded dict %s.' % dict_prefix)

    def get_schema_list(self):
        order = sorted(self.__schema.keys(), key=lambda s: self.__schema[s][-1], reverse=True)
        return [{'schema': s, 'displayName': self.__schema[s][0]} for s in order]

    def get_schema(self, schema):
        for s in self.__schema:
            if s == schema:
                t = self.__schema[s]
                t[-1] += 1
                self.__load_dict(t[1])
                return {'config': t[2], 'spellingMap': t[3]}
        return None
        
    """
    def lookup_freq_total(self):
        return (0, 0)

    def lookup_unigram(self, key):
        #print 'lookup_unigram:', key
        return None

    def lookup_bigram(self, key):
        #print 'lookup_bigram:', key
        return None

    def update_freq_total(self, n):
        #print 'update_freq_total:', n
        pass
        
    def update_unigram(self, e):
        #print 'update_unigram:', unicode(e)
        pass

    def update_bigram(self, a, b, get_ikeys):
        #print 'update_bigram:', unicode(a), unicode(b)
        pass
        k_ids = [self.__get_or_insert_key(k) for k in get_ikeys(a, b)]
        for k_id in k_ids:
            self.__add_kb(k_id, b_id)

    def __get_or_insert_key(self, key):
        args = {'ikey' : key}
        r = None
        while not r:
            r = DB.__conn.execute(self.QUERY_KEY_SQL, args).fetchone()
            if not r:
                DB.__conn.execute(self.ADD_KEY_SQL, args)
        return r[0]

    def __add_kb(self, k_id, b_id):
        args = {'k_id' : k_id, 'b_id' : b_id}
        if not DB.__conn.execute(self.QUERY_KB_SQL, args).fetchone():
            DB.__conn.execute(self.ADD_KB_SQL, args)
    """


def main():
    pass

if __name__ == '__main__':
    main()
