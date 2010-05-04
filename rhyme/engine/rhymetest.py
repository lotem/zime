#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim:set et sts=4 sw=4:

import socket 
import cStringIO
from ctypes import *

import ibus
from ibus import keysyms
from ibus import modifier

ERROR_ALREADY_EXISTS = 183
MUTEX_ALL_ACCESS = 0x1F0001
SW_HIDE = 0

HOST, PORT = "localhost", 2133
MUTEX_NAME = "RhymeService"
EVENT_NAME = "RhymeServiceReadyEvent"

class TestEngine:

    def __init__(self, schema):
        self.__lookup_table = ibus.LookupTable()
        self.__sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        if not self.__check_service():
            return
        self.__sock.connect((HOST, PORT))
        self.__sock.send("INIT=%s\n" % schema)

    def __check_service(self):
        mutex = windll.kernel32.OpenMutexA(MUTEX_ALL_ACCESS, False, MUTEX_NAME)
        if mutex:
            print "service already started."
            windll.kernel32.CloseHandle(mutex)
            return True
        return self.__start_service()

    def __start_service(self):
        event = windll.kernel32.CreateEventA(None, True, False, EVENT_NAME)
        if not event:
            print "error creating event %s." % EVENT_NAME
            return False
        if windll.kernel32.GetLastError() == ERROR_ALREADY_EXISTS:
            print "waiting for service to be ready..."
        else:
            print "lauching Rhyme service...",
            handle = windll.shell32.ShellExecuteA(None, "open", "pythonw.exe", "rhyme.py", None, SW_HIDE)
            if handle <= 32:
                print "error launching service: %d" % handle
                windll.kernel32.CloseHandle(event)
                return False
        windll.kernel32.WaitForSingleObject(event, 15000)
        print "ok."
        windll.kernel32.CloseHandle(event)
        return True

    def close(self):
        self.__sock.close()

    def process_key_event(self, keycode, mask):
        print "process_key_event: '%s'(%x), %08x" % (keysyms.keycode_to_name(keycode), keycode, mask)
        self.__sock.send("EVENT=%d,%d\n" % (keycode, mask))
        ret = False
        f = self.__sock.makefile("r")
        line = f.readline()
        while line:
            cmd, params = line.rstrip("\n").split("=", 1)
            print "RESPONSE: %s=%s" % (cmd, params.decode('utf-8'))
            if cmd == "RET":
                ret = params == "true"
                break
            elif cmd == "CANDIDATES":
                if params == "0":
                    print "NO CANDIDATE."
                else:
                    n, c, t = map(int, params.split(","))
                    for i in range(n):
                        print "CANDIDATE:", i + 1, f.readline().rstrip("\n").decode("utf-8")
            line = f.readline()
        f.close()
        return ret

    def test(self, string):
        name = ''
        is_name = False
        for c in string:
            if c == '{':
                name = ''
                is_name = True
            elif c == '}':
                is_name = False
                self.process_key_event(keysyms.name_to_keycode(name), 0)
            elif is_name:
                name += c
            else:
                self.process_key_event(ord(c), 0)

def main():
    # test schema chooser menu
    #e.process_key_event(keysyms.grave, modifier.CONTROL_MASK)  # Ctrl+grave
    #e.test('2')

    #e = TestEngine(u'Zhuyin')
    #e.test('rm/3rm/3u.3gp6zj/ {Escape}2k7al {Tab}{Return}')

    #e = TestEngine(u'Pinyin')
    #e.test("pinyin-shuru'fa' ")
    #e.test('henanquan{Home}{Tab} ')

    #e = TestEngine(u'ComboPinyin')
    #e.process_key_event(keysyms.r, 0)
    #e.process_key_event(keysyms.j, 0)
    #e.process_key_event(keysyms.k, 0)
    #e.process_key_event(keysyms.l, 0)
    #e.process_key_event(keysyms.r, modifier.RELEASE_MASK)
    #e.process_key_event(keysyms.j, modifier.RELEASE_MASK)
    #e.process_key_event(keysyms.k, modifier.RELEASE_MASK)
    #e.process_key_event(keysyms.l, modifier.RELEASE_MASK)
    #e.process_key_event(keysyms.space, 0)
    #e.process_key_event(keysyms.space, modifier.RELEASE_MASK)

    e = TestEngine(u'Jyutping')
    e.test('jyuhomindeoicangjatheizaugwodikjatzi ')
    #e.test('fanhoifongziganbunsamgikci')

    e.close()

    pass

if __name__ == "__main__":
    main()
