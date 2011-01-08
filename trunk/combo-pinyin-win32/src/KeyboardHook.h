// KeyboardHook.h
//
// ZimeLib Header
//
// Copyright (C) 2007
//
// Author: GONG Chen
// E-mail: contralisk@mail.com
// Project Home: http://zime.googlecode.com
// Licence: GPL v3
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "KeyEventFilter.h"
#include <boost/signals/connection.hpp>

class KeyboardHook : public KeyEventFilter
{
public:
	static KeyboardHook& get_instance();

	KeyboardHook::~KeyboardHook() { uninstall_hook(); }

	virtual void on_key_event(KeyEvent& key_event);

	bool install_hook(HINSTANCE hInstance);
	bool uninstall_hook();

private:
	KeyboardHook() : hook_(NULL) {}

	static LRESULT CALLBACK keyboard_proc(int nCode, WPARAM wParam, LPARAM lParam);
	static void send_input(KeyEvent const& key_event);

	HHOOK hook_;
	boost::signals::connection sink_conn_;
};
