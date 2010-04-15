#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim:set et sts=4 sw=4:

import ibus
from ibus import keysyms
from ibus import modifier

import zimeengine

class RhymeEngine:

    def __init__(self, frontend):
        self.__lookup_table = ibus.LookupTable()
        self.__frontend = frontend
        self.__backend = zimeengine.SchemaChooser(self)

    def process_key_event(self, keycode, mask):
        #print "process_key_event: '%s'(%x), %08x" % (keysyms.keycode_to_name(keycode), keycode, mask)
        return self.__backend.process_key_event(keycode, mask)

    def commit_string(self, s):
        #print u'commit: [%s]' % s
        self.__frontend.commit_string(ibus.Text(s))

    def update_preedit(self, s, start, end):
        #print u'preedit: [%s[%s]%s]' % (s[:start], s[start:end], s[end:])
        if not s:
            self.__frontend.hide_preedit()
            return
        self.__frontend.update_preedit(ibus.Text(s), start, end)

    def update_aux_string(self, s):
        #print u'aux: [%s]' % s
        if not s:
            self.__frontend.hide_aux_string()
            return
        self.__frontend.update_aux_string(ibus.Text(s))

    def update_candidates(self, candidates):
        self.__lookup_table.clean()
        self.__lookup_table.show_cursor(False)
        if not candidates:
            self.__frontend.hide_candidates()
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
        self.__frontend.update_candidates(candidates, current_page, total_number_of_pages)
            
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

