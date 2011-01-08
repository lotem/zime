// ComboFilter.cpp
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
#include "ComboFilter.h"
#include "EventDef.h"
#include <boost/bind.hpp>

ComboFilter::ComboFilter( Timer& timer )
	: timer_(timer),
	shift_state_(RELEASED), 
	caps_state_(RELEASED), 
	space_state_(RELEASED),
	repeating_(false), 
	enabled_(true), 
	n_keys_held_(0), 
	combo_(), 
	n_commited_(1)
{
	next.connect(sink);
}

ComboFilter::~ComboFilter()
{
	timer_.kill();
}

void ComboFilter::on_key_event(KeyEvent& key_event)
{
	KeyEvent::KeyInfoType key_info = key_event.get_key_info();

	wchar_t ch = static_cast<wchar_t>(key_info->vkCode);
	bool key_up = (key_info->flags & LLKHF_UP) != 0;

	handle_shift(ch, key_up);

	if(key_info->flags & (LLKHF_EXTENDED | LLKHF_INJECTED | LLKHF_ALTDOWN))
	{
		return;
	}

	if (ch == VK_CAPITAL)
	{
		handle_caps(key_up, key_event);
		return;
	}

	if (caps_state_ > RELEASED)
	{
		handle_caps_combo(ch, key_up, key_event);
		return;
	}

	if (ch == VK_SPACE)
	{
		handle_ctrl_space(key_up);
	}

	if (!enabled_)
	{
		return;
	}

	if (!is_chinese_ime_active())
	{
		return;
	}

	if(	is_key_down(VK_SHIFT) ||
		is_key_down(VK_CONTROL) ||
		is_key_down(VK_MENU) ||
		is_key_down(VK_LWIN) ||
		is_key_down(VK_RWIN) )
	{
		reset_combo();
		n_commited_ = 1;
		return;
	}

	//
	if (enhanced_bksp_ && ch == VK_BACK)
	{
		handle_enhanced_bksp(key_up, key_event);
		return;
	}

	wstring::size_type pos = p_config_->xlit_from.find(ch);

	// keys of no interest
	if(pos == wstring::npos) {
		reset_combo();
		n_commited_ = 1;
		return;
	}
	
	handle_combo_key(key_up, pos, key_event);
}

bool ComboFilter::is_chinese_ime_active() const
{
	HWND hWnd = GetForegroundWindow();
	DWORD idThread = GetWindowThreadProcessId(hWnd, NULL);
	HKL hkl = GetKeyboardLayout(idThread);
	// TRICKY !
	if ((DWORD)hkl & 0xA0000000)
		return true;
	else
		return false;
}

bool ComboFilter::is_key_down(UINT vk) const
{
	return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

void ComboFilter::on_timer()
{
	if (n_keys_held_ > 0) 
	{
		// if repeating, don not send combo_ on key release.
		repeating_ = true;
	}
	send_combo();
}

void ComboFilter::handle_shift( wchar_t ch, bool key_up )
{
	if (ch == VK_LSHIFT)
	{
		////lotem removed in v1.2
		//if (key_up && shift_state_ == KEY_DOWN &&
		//	!is_key_down(VK_CONTROL) && 
		//	!is_key_down(VK_MENU))
		//{
		//	// released without having been used in combo_
		//	Toggle();
		//}
		shift_state_ = key_up ? RELEASED : (shift_state_ == RELEASED ? KEY_DOWN : COMBINED);
		// let go...
	}
	else if (shift_state_ > RELEASED)
	{
		shift_state_ = COMBINED;
		// let go...
	}
}

void ComboFilter::handle_ctrl_space( bool key_up )
{
	key_state previous = space_state_;
	space_state_ = key_up ? RELEASED : KEY_DOWN;

	if (!key_up && previous == RELEASED &&
		is_key_down(VK_CONTROL) &&
		!is_key_down(VK_SHIFT) && 
		!is_key_down(VK_MENU))
	{
		// Ctrl+Space
		if (p_config_->opt_ctrl_space_switch)
		{
			if (is_chinese_ime_active() == enabled_)
				toggle();
		}
	}
}

void ComboFilter::handle_caps( bool key_up, KeyEvent &key_event )
{
	if (key_up)
	{
		if (caps_state_ == KEY_DOWN) 
		{
			// released without having been used in combination
			if (shift_state_ > RELEASED)
				send_vkcodes(wstring(1, VK_CAPITAL));
		}
		caps_state_ = RELEASED;
	}
	else 
	{
		if (caps_state_ < KEY_DOWN)
			caps_state_ = KEY_DOWN;
	}
	key_event.consumed(true);
}

void ComboFilter::handle_caps_combo( wchar_t ch, bool key_up, KeyEvent &key_event )
{
	caps_state_ = COMBINED;

	if (ch == VK_BACK)
	{
		if (key_up)
		{					
			enhanced_bksp_ = !enhanced_bksp_;
		}
		key_event.consumed(true);
		return;
	}

	//// lotem added in v1.2
	if (ch == VK_SPACE)
	{
		if (key_up)
		{	
			toggle();
		}
		key_event.consumed(true);
		return;
	}

	handle_nav_key(ch, key_up, key_event);
}

void ComboFilter::handle_nav_key( wchar_t ch, bool key_up, KeyEvent &key_event )
{
	wstring::size_type pos = p_config_->nav_from.find(ch);
	if (pos != wstring::npos) {
		if(!key_up)
		{
			send_vkcodes(wstring(1, p_config_->nav_to[pos]));
		}
	}
	key_event.consumed(true);	
}

void ComboFilter::handle_enhanced_bksp( bool key_up, KeyEvent &key_event )
{
	if (!key_up)
	{					
		send_vkcodes(wstring(n_commited_, VK_BACK));
		reset_combo();
		n_commited_ = 1;
	}
	key_event.consumed(true);
}

void ComboFilter::handle_combo_key( bool key_up, wstring::size_type pos, KeyEvent &key_event )
{
	if(key_up) 
	{
		if (combo_[pos])
		{
			--n_keys_held_;
		}

		if (n_keys_held_ <= 0)
		{
			if (repeating_)
			{
				repeating_ = false;
			}
			else
			{
				send_combo();
			}
			reset_combo();
		}
	}
	else // key down 
	{ 
		if (!combo_[pos])
		{
			combo_[pos] = true;
			++n_keys_held_;
			if(p_config_->opt_repeat)
			{
				timer_.kill();
				timer_.set(DELAY, boost::bind(&ComboFilter::on_timer, this));
			}
		}
	}
	key_event.consumed(true);
}

void ComboFilter::send_combo()
{
	wstring text;
	for (wstring::size_type i = 0; i < combo_.size(); ++i)
	{
		if (combo_[i])
		{
			text += p_config_->xlit_to[i];
		}
	}
	if (!text.empty())
	{
		if (p_config_)
		{
			text = p_config_->apply_rules(text);
		}
		
		if (text == L"~")
		{
			send_vkcodes(wstring(enhanced_bksp_ ? n_commited_ : 1, VK_BACK));
			n_commited_ = 1;
		} 
		else 
		{
			send_vkcodes(text);
		}
	}
}

void ComboFilter::reset_combo()
{
	timer_.kill();
	n_keys_held_ = 0;
	combo_.clear();
	combo_.resize(p_config_->xlit_from.length());
}

void ComboFilter::send_vkcodes( const wstring& vkcodes )
{
	if (vkcodes.empty())
		return;

	KeyEvent result;

	KeyEvent::InputType input;
	foreach(wchar_t ch, vkcodes)
	{
		input.type = INPUT_KEYBOARD;
		input.ki.wVk = ch;
		input.ki.wScan = 0;
		input.ki.dwFlags = 0;
		input.ki.time = 0;
		input.ki.dwExtraInfo = 0;
		result.add_translation(input);

		input.type = INPUT_KEYBOARD;
		input.ki.wVk = ch;
		input.ki.wScan = 0;
		input.ki.dwFlags = KEYEVENTF_KEYUP;
		input.ki.time = 0;
		input.ki.dwExtraInfo = 0;
		result.add_translation(input);
	}

	next(result);
	n_commited_ = vkcodes.length();
}

void ComboFilter::configure( const shared_ptr<ComboConfig>& p_config )
{
	p_config_ = p_config;
	enabled_ = p_config->opt_enabled;
	enhanced_bksp_ = p_config->opt_enhanced_bksp;
	reset_combo();
}

void ComboFilter::toggle()
{
	enabled_ = !enabled_;
	n_commited_ = 1;
	// alter display for IME state change
	notify(TOGGLE, (UINT)enabled_);
}
