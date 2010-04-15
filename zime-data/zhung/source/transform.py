#! /usr/bin/env python

import re
import fileinput

config = '''
gi-j !zi-j v-w bu zhi zr-zh !zr-z !rh-r !zh-z !l-n
ueng-ung ien-ian uon-uan m-n !ng-n rhy-el o-eo ptk-h 
!5-tones 4-tones !2-tones !1-tone
'''.split()

RULES = {
    'gi-j' : [
        (r'^gi', r'ji'),
        (r'^ki', r'qi'),
        (r'^hi', r'xi'),
        (r'^([jqx])i([aoeu])', r'\1\2'),
    ],
    'zi-j' : [
        (r'^zi', r'ji'),
        (r'^ci', r'qi'),
        (r'^si', r'xi'),
        (r'^([jqx])i([aoeu])', r'\1\2'),
    ],
    'v-w' : [
        (r'^v([aoeu])', r'w\1'),
        #(r'^vi', r'wui'),
    ],
    'bu' : [
        (r'^([bpmfv])un(\w*)$', r'\1en\2'),
        (r'^([bpm])uon(\d?)$', r'\1an\2'),
        #(r'^([bpm])u([ae]\w*)$', r'\1\2'),
    ],
    'zhi' : [
        (r'^([zcsr]h)i(\d?)$', r'\1y\2'),
        (r'^([zcsr]h)i([mn]\w*)$', r'\1e\2'),
        (r'^([zcsr]h)i(u?)e([mn]\d?)$', r'\1\2a\3'),
        (r'^([zcsr]h)i(u?)e([ptkh]?\d?)$', r'\1\2o\3'),
        (r'^([zcsr]h)i([aoeu]\w*)$', r'\1\2'),
        #(r'^([zcs])ri(\d?)$', r'\1hy\2'),
        #(r'^([zcs]r)i([mn]\w*)$', r'\1e\2'),
        #(r'^([zcs]r)i(u?)e([mn]\d?)$', r'\1\2a\3'),
        (r'^([zcs]r)i(u?)e([ptkh]?\d?)$', r'\1\2o\3'),
        #(r'^([zcs]r)i([aoeu]\w*)$', r'\1\2'),
    ],
    'zr-zh' : [
        (r'^([zcs])r', r'\1h'),
    ],
    'zr-z' : [
        (r'^([zcs])r', r'\1'),
    ],
    'rh-r' : [
        (r'^rh([aei])', r'r\1'),
        (r'^rhou', r'rou'),
    ],
    'zh-z' : [
        (r'^([zcsr])h', r'\1'),
        (r'^([zcs])r', r'\1'),
    ],
    'l-n' : [
        (r'^l', r'n'),
    ],
    'ueng-ung' : [
        #(r'weng(\d?)$', r'wung\1'),
        (r'ueng(\d?)$', r'ung\1'),
        (r'([yi])uek(\d?)$', r'\1uk\2'),
    ],
    'ien-ian' : [
        (r'([jqxyi]u?)e([mn]\d?)$', r'\1a\2'),
    ],
    'uon-uan' : [
        (r'([wu])o(n\d?)$', r'\1a\2'),
    ],
    'm-n' : [
        (r'm(\d?)$', r'n\1'),
    ],
    'ng-n' : [
        (r'ng(\d?)$', r'n\1'),
    ],
    'rhy-el' : [
        (r'^rh?y(\d?)$', r'el\1'),
    ],
    'o-eo' : [
        (r'(^|[^jqxyiwu])o([ptkh]?\d?)$', r'\1eo\2'),
    ],
    'ptk-h' : [
        (r'[ptk](\d?)$', r'h\1'),
    ],
    '5-tones' : [
        (r'([ptkh])2$', r'\1'),
    ],
    '4-tones' : [
        (r'[ptkh]2$', r'2'),
        (r'[ptkh]$', r'1'),
    ],
    '2-tones' : [
        (r'\d$', r''),
    ],
    '1-tone' : [
        (r'\d$', r''),
        (r'[ptkh]$', r''),
    ],
}

selected_rules = []

def compile_rules():
    for r in config:
        if r.startswith('!'):
            continue
        if r not in RULES:
            continue
        for p in RULES[r]:
            selected_rules.append((re.compile(p[0]), p[1]))

def apply_rules(s):
    for p in selected_rules:
        s = p[0].sub(p[1], s)
    return s

def output(append=False):
    t = set()
    for line in fileinput.input():
        words = line.rstrip('\n').split('\t')
        #assert len(words) == 12
        s = words[-1]
        if s:
            s = apply_rules(s)
        if s not in t:
            t.add(s)
        if append:
            words.append(s)
        else:
            words = [s, words[1]]
        print '\t'.join(words)
    print '# distinct keywords: %d' % len(t)
    for s in sorted(t):
        print '# %s' % s

compile_rules()
output()
