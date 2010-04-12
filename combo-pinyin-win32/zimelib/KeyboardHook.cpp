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

HHOOK KeyboardHook::hook_ = NULL;

KeyboardHook& KeyboardHook::Instance() 
{ 	
	static KeyboardHook _instance;
	return _instance; 
}

KeyboardHook::~KeyboardHook()
{
	UninstallHook();
}

bool KeyboardHook::InstallHook(HINSTANCE hInstance)
{
	if (hook_ != NULL) 
		return false;
	hook_ = SetWindowsHookEx(WH_KEYBOARD_LL, keyboard_proc, hInstance, NULL);
	if (hook_ == NULL) 
		return false;

	conn_to_sink_ = Sink.connect(&KeyboardHook::send_input);
	return true;
}

bool KeyboardHook::UninstallHook()
{
	if (hook_ != NULL) {
		if (UnhookWindowsHookEx(hook_) == 0) 
			return false;
		hook_ = NULL;
	}

	conn_to_sink_.disconnect();
	return true;
}

void KeyboardHook::OnKey(KeyEvent& key_event)
{
	Passage(key_event);
}

LRESULT CALLBACK KeyboardHook::keyboard_proc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= HC_ACTION)
	{
		PKBDLLHOOKSTRUCT pStruct = (PKBDLLHOOKSTRUCT)lParam;
		switch (wParam)
		{
		case WM_KEYDOWN:
		case WM_KEYUP:
			{
				KeyEvent key_event(pStruct);
				Instance().OnKey(key_event);
				if (key_event.Consumed())
					return true;
			}
			break;

		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			break;
		}
	}
	return CallNextHookEx(hook_, nCode, wParam, lParam);
}

void KeyboardHook::send_input(KeyEvent const& key_event)
{
	KeyEvent::InputSequence inputs = key_event.Translation();
	UINT cInput = (UINT)inputs.size();
	if (cInput == 0)
		return;

	LPINPUT pInput = new INPUT[cInput];
	std::copy(inputs.begin(), inputs.end(), pInput);

	::SendInput(cInput, pInput, sizeof(INPUT));
	delete[] pInput;
}
