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
	static KeyboardHook& Instance();
	~KeyboardHook();
	bool InstallHook(HINSTANCE hInstance);
	bool UninstallHook();
	virtual void OnKey(KeyEvent& key_event);
private:
	KeyboardHook() {}
	static LRESULT CALLBACK keyboard_proc(int nCode, WPARAM wParam, LPARAM lParam);
	static void send_input(KeyEvent const& key_event);
	static HHOOK hook_;
	boost::signals::connection conn_to_sink_;
};
