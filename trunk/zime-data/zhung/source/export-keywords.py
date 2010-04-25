#! /usr/bin/env python

import re
import fileinput

def main():
    first = True
    for line in fileinput.input():
        if first:
            first = False
            continue
        r = line.rstrip('\n').split('\t')
        assert len(r) == 7
        z, t, j = r[1], r[3], r[4]
        if j:
            if j == '!':
                continue
            if j == '*':
                print '%s\t*%s' % (t, z)
            else:
                a = j.split(' ')
                for s in a:
                    if s.endswith('*'):
                        print '%s\t*%s' % (s[:-1], z)
                    else:
                        print '%s\t%s' % (s, z)
        else:
            print '%s\t%s' % (t, z)

if __name__ == "__main__":
    main()
