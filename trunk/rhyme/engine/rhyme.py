#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim:set et sts=4 sw=4:

import os
import logging
import SocketServer

import ibus
from ibus import keysyms
from ibus import modifier

import zimeengine
import zimeparser
from zimedb import DB

def _initialize():
    zimeparser.register_parsers()
    # initialize DB 
    HOME_PATH = os.getenv('HOME')
    if HOME_PATH:
        db_path = os.path.join(HOME_PATH, '.ibus', 'zime')
    else:
        db_path = '.'
    user_db = os.path.join(db_path, 'zime.db')
    if not os.path.exists(user_db):
        if not os.path.isdir(db_path):
            os.makedirs(db_path)
    DB.open(user_db)

_initialize()

class RhymeSessionHandler(SocketServer.StreamRequestHandler):

    def handle(self):
        for line in self.rfile:
            tag, params = line.rstrip("\n").split("=", 1)
            if tag == "INIT":
                logging.info("init session:", params)
                self.__lookup_table = ibus.LookupTable()
                self.__engine = zimeengine.SchemaChooser(self, params)
            if tag == "EVENT":
                keycode, mask = map(int, params.split(","))
                logging.debug("process_key_event: '%s'(%x), %08x" % (keysyms.keycode_to_name(keycode), keycode, mask))
                ret = self.__engine.process_key_event(keycode, mask)
                self.wfile.write("RET=%s\n" % ("true" if ret else "false"))
                self.wfile.flush()

    def commit_string(self, s):
        logging.debug(u'commit: [%s]' % s)
        self.wfile.write("COMMIT=%s\n" % s.encode('utf-8'))

    def update_preedit(self, s, start, end):
        logging.debug(u'preedit: [%s[%s]%s]' % (s[:start], s[start:end], s[end:]))
        self.wfile.write("PREEDIT=%s\n" % s.encode('utf-8'))
        self.wfile.write("CURSOR=%d,%d\n" % (start, end))

    def update_aux_string(self, s):
        logging.debug(u'aux: [%s]' % s)
        self.wfile.write("AUX=%s\n" % s.encode('utf-8'))

    def update_candidates(self, candidates):
        self.__lookup_table.clean()
        self.__lookup_table.show_cursor(False)
        if not candidates:
            self.wfile.write("CANDIDATES=0\n")
        else:
            for c in candidates:
                self.__lookup_table.append_candidate(ibus.Text(c[0]))
            self.__update_page()

    def __update_page(self):
        candidates = self.__lookup_table.get_candidates_in_current_page()
        n = self.__lookup_table.get_number_of_candidates()
        c = self.__lookup_table.get_cursor_pos()
        p = self.__lookup_table.get_page_size()
        current_page = c / p
        total_number_of_pages = (n + p - 1) / p
        self.wfile.write("CANDIDATES=%d,%d,%d\n" % (len(candidates), current_page, total_number_of_pages))
        for x in candidates:
            self.wfile.write(x.get_text().encode('utf-8') + "\n")
            
    def page_up(self):
        if self.__lookup_table.page_up():
            #print u'page_up.'
            self.__update_page()
            return True
        return False

    def page_down(self):
        if self.__lookup_table.page_down():
            #print u'page_down.'
            self.__update_page()
            return True
        return False

    def cursor_up(self):
        if self.__lookup_table.cursor_up():
            #print u'cursor_up.'
            self.__update_page()
            return True
        return False

    def cursor_down(self):
        if self.__lookup_table.cursor_down():
            #print u'cursor_down.'
            self.__update_page()
            return True
        return False

    def get_candidate_index(self, index):
        index += self.__lookup_table.get_current_page_start()
        #print u'index = %d' % index
        return index

    def get_candidate_cursor_pos(self):
        index = self.__lookup_table.get_cursor_pos()
        #print u'candidate_cursor_pos = %d' % index
        return index

class RhymeService:
    
    HOST, PORT = "localhost", 2133

    def __init__(self):
        addr = (RhymeService.HOST, RhymeService.PORT)
        self.server = SocketServer.TCPServer(addr, RhymeSessionHandler)

    def run(self):
        logging.info("RhymeService serving...")
        self.server.serve_forever()

def main():
    service = RhymeService()
    service.run()

if __name__ == "__main__":
    main()
