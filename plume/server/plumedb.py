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
                logging.error('error parsing (%s) %s' % (schema_file, x))
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
        # [display_name, dict_prefix, config, spelling_map, use_count]
        self.__schema[schema] = [display_name, dict_prefix, config, None, 0]
        logging.info('loaded schema %s.' % schema)
        self.__register_dict(dict_prefix, max_key_length, (schema, mapping_rules, fuzzy_rules, spelling_rules, alternative_rules))

    def __register_dict(self, dict_prefix, max_key_length, schema_info):
        if dict_prefix not in self.__dict:
            # [max_key_length, associated_schemas, io_map, oi_map, keywords, u_index, b_index, freq_total]
            s = self.__dict[dict_prefix] = [max_key_length, [], None, None, list(), dict(), dict(), 0]
        else:
            s = self.__dict[dict_prefix]
        s[1].append(schema_info)

    def __load_dict(self, dict_prefix):
        if dict_prefix not in self.__dict:
            return
        d = self.__dict[dict_prefix]
        if d[1] == None:
            return
        keyword_file = os.path.join(self.DATA_DIR, '%s-keywords.txt' % dict_prefix)
        if not os.path.exists(keyword_file):
            logging.error('missing keyword file %s.' % keyword_file)
            return
        keywords = d[4]
        keyword_map = dict()
        f = open(keyword_file, 'r')
        for line in f:
            x = line.strip().decode('utf-8')
            if not x or x.startswith(u'#'):
                continue
            try:
                k = x.split(u'\t', 1)
                (okey, phrase) = k
            except:
                logging.error('invalid format in %s: %s' % (keyword_file, x))
                continue
            i = keyword.index(k)
            if i == -1:
                i = len(keywords)
                keywords.append(k)
            if okey not in keyword_map:
                keyword_map[okey] = [i]
            else:
                keyword_map[okey].append(i)
        f.close()
        sa = SpellingAlgebra()
        first = True
        for s in d[1]:
            try:
                schema, mapping_rules, fuzzy_rules, spelling_rules, alternative_rules = s
                spelling_map, io_map, oi_map = sa.calculate(mapping_rules, 
                                                            fuzzy_rules, 
                                                            spelling_rules, 
                                                            alternative_rules, 
                                                            keyword_map)
            except SpellingCollisionError, e:
                logging.error('spelling collision error in schema %s: %s' % (schema, e))
                continue
            self.__schema[schema][3] = spelling_map
            if first:
                first = False
                d[2], d[3] = io_map, oi_map
        d[1] = None
        logging.info('loaded dict %s.' % dict_prefix)
        self.__populate_u_index(d, keyword_map)
        #phrase_file = os.path.join(self.DATA_DIR, '%s-phrases.txt' % dict_prefix)
        #if os.path.exists(phrase_file):
        #    self.__schedule_training_task(d, phrase_file)

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
        
    def __process_phrase_func(self, d):
        max_key_length, schemas, io_map, oi_map, keywords, u_index, b_index, freq_total = d
        def g(ikeys, okey, depth):
            if not okey or depth >= max_key_length:
                return ikeys
            r = []
            for x in ikeys:
                if okey[0] not in oi_map:
                    if options.verbose:
                        logging.error(u'invalid keyword encountered: [%s]' % okey[0])
                    return []
                for y in oi_map[okey[0]]:
                    r.append(x + [y])
            return g(r, okey[1:], depth + 1)
        def process_phrase(okey, phrase, freq):
            d[-1] += freq
            k = (okey, phrase)
            if k in freq_map:
                freq_map[k] += freq
            else:
                freq_map[k] = freq
                # generate index
                ikeys = g([[]], okey.split(), 0)
                if not ikeys and options.verbose:
                    logging.error(u'failed index generation for phrase [%s] %s.' % (okey, phrase))
                for i in ikeys:
                    ikey = u' '.join(i)
                    if ikey in index:
                        index[ikey].add(k)
                    else:
                        index[ikey] = set([k])
        return process_phrase

    def __populate_u_index(self, d, keyword_map):
        # TODO: handle this in task queue
        process_phrase = self.__process_phrase_func(d)
        for okey in keyword_map:
            for phrase in keyword_map[okey]:
                process_phrase(okey, phrase, 0)
        
    def __schedule_training_task(self, d, filename):
        # TODO: handle this in task queue
        process_phrase = self.__process_phrase_func(d)
        for line in open(filename):
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
                logging.error(u'invalid format in %s: %s' % (filename, x))
                continue
            process_phrase(okey, phrase, freq)

    def get_dict_info(self, schema):
        return self.__dict[self.__schema[schema][1]]


    def lookup_unigram(self, d, key):
        logging.debug('lookup_unigram: %s' % key)
        dict_index, freq_map = d[4], d[5]
        return [(k[0], k[1], freq_map[k]) for k in dict_index[key]] if key in dict_index else []

    def lookup_bigram(self, d, key):
        logging.debug('lookup_bigram: %s' % key)
        return None

    """
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
