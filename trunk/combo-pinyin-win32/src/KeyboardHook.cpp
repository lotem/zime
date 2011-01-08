// KeyboardHook.cpp
//
// ZimeLib Source
//
// Copyright (C) 2007
//
// Author: GONG Chen
// E-mail: contralisk@mail.com
// Project Home: http://zime.googlecode.com
// Licence: GPL v3
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KeyboardHook.h"

KeyboardHook& KeyboardHook::get_instance() 
{ 	
	static KeyboardHook s_instance;
	return s_instance; 
}

bool KeyboardHook::install_hook(HINSTANCE hInstance)
{
	if (hook_ != NULL) 
		return false;
	hook_ = SetWindowsHookEx(WH_KEYBOARD_LL, keyboard_proc, hInstance, NULL);
	if (hook_ == NULL) 
		return false;

	sink_conn_ = sink.connect(&KeyboardHook::send_input);
	return true;
}

bool KeyboardHook::uninstall_hook()
{
	if (hook_ != NULL) {
		if (UnhookWindowsHookEx(hook_) == 0) 
			return false;
		hook_ = NULL;
	}

	sink_conn_.disconnect();
	return true;
}

void KeyboardHook::on_key_event(KeyEvent& key_event)
{
	next(key_event);
}

LRESULT CALLBACK KeyboardHook::keyboard_proc(int nCode, WPARAM wParam, LPARAM lParam)
{
	KeyboardHook& instance = get_instance();
	if (nCode >= HC_ACTION)
	{
		PKBDLLHOOKSTRUCT p_struct = (PKBDLLHOOKSTRUCT)lParam;
		switch (wParam)
		{
		case WM_KEYDOWN:
		case WM_KEYUP:
			{
				KeyEvent key_event(p_struct);
				instance.on_key_event(key_event);
				if (key_event.consumed())
					return true;
			}
			break;

		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			break;
		}
	}
	return CallNextHookEx(instance.hook_, nCode, wParam, lParam);
}

void KeyboardHook::send_input(KeyEvent const& key_event)
{
	KeyEvent::InputSequence trans = key_event.get_translation();
	UINT count = (UINT)trans.size();
	if (count == 0)
		return;

	KeyEvent::InputType* p_input = new KeyEvent::InputType[count];
	std::copy(trans.begin(), trans.end(), p_input);

	::SendInput(count, p_input, sizeof(KeyEvent::InputType));
	delete[] p_input;
}
