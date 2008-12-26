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
	enabled_(false), 
	n_keys_held_(0), 
	comb_(), 
	n_commited_(1)
{
	Passage.connect(Sink);
}

ComboFilter::~ComboFilter()
{
	timer_.Kill();
}

void ComboFilter::OnKey(KeyEvent& key_event)
{
	KeyEvent::KeyInfoType key_info = key_event.KeyInfo();

	wchar_t ch = static_cast<wchar_t>(key_info->vkCode);
	bool key_up = (key_info->flags & LLKHF_UP) != 0;

	handle_shift(ch, key_up);

	if(key_info->flags & (LLKHF_EXTENDED | LLKHF_INJECTED | LLKHF_ALTDOWN))
	{
		return;
	}

	if (ch == VK_CAPITAL)
	{
		handle_caps_key(key_up, key_event);
		return;
	}

	if (caps_state_ > RELEASED)
	{
		handle_caps_comb(ch, key_up, key_event);
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

	if(	is_key_down(VK_SHIFT) ||
		is_key_down(VK_CONTROL) ||
		is_key_down(VK_MENU) ||
		is_key_down(VK_LWIN) ||
		is_key_down(VK_RWIN) )
	{
		reset_comb();
		n_commited_ = 1;
		return;
	}

	//
	if (opt_enhanced_bksp_ && ch == VK_BACK)
	{
		handle_enhanced_bksp_key(key_up, key_event);
		return;
	}

	wstring::size_type pos = xlit_from_.find(ch);

	// keys of no interest
	if(pos == wstring::npos) {
		reset_comb();
		n_commited_ = 1;
		return;
	}
	
	handle_comb_key(key_up, pos, key_event);
}

bool ComboFilter::is_key_down(UINT vk)
{
	return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

void ComboFilter::on_timer()
{
	if (n_keys_held_ > 0) 
	{
		// if repeating, don not send comb_ on key release.
		repeating_ = true;
	}
	send_comb();
}

void ComboFilter::handle_shift( wchar_t ch, bool key_up )
{
	if (ch == VK_LSHIFT)
	{
		////lotem removed in v1.2
		//if (key_up && shift_state_ == HELD_DOWN &&
		//	!is_key_down(VK_CONTROL) && 
		//	!is_key_down(VK_MENU))
		//{
		//	// released without having been used in comb_
		//	Toggle();
		//}
		shift_state_ = key_up ? RELEASED : (shift_state_ == RELEASED ? HELD_DOWN : COMBINED);
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
	space_state_ = key_up ? RELEASED : HELD_DOWN;

	// lotem modified in v1.2s
	if (!key_up && previous == RELEASED &&
		is_key_down(VK_CONTROL) &&
		!is_key_down(VK_SHIFT) && 
		!is_key_down(VK_MENU) &&
		opt_ctrl_space_toggles_)
	{
		// Ctrl+Space
		Toggle();
	}
}

void ComboFilter::handle_caps_key( bool key_up, KeyEvent &key_event )
{
	if (key_up)
	{
		if (caps_state_ == HELD_DOWN) 
		{
			// released without having been used in combination
			send_vkcodes(wstring(1, VK_CAPITAL));
		}
		caps_state_ = RELEASED;
	}
	else 
	{
		if (caps_state_ < HELD_DOWN)
			caps_state_ = HELD_DOWN;
	}
	key_event.Consumed(true);
}

void ComboFilter::handle_caps_comb( wchar_t ch, bool key_up, KeyEvent &key_event )
{
	caps_state_ = COMBINED;

	if (ch == VK_BACK)
	{
		if (key_up)
		{					
			opt_enhanced_bksp_ = !opt_enhanced_bksp_;
		}
		key_event.Consumed(true);
		return;
	}

	//// lotem added in v1.2
	if (ch == VK_SPACE)
	{
		if (key_up)
		{	
			if (is_key_down(VK_CONTROL))
			{
				opt_ctrl_space_toggles_ = !opt_ctrl_space_toggles_;
			}
			else
			{
				Toggle();
			}
		}
		key_event.Consumed(true);
		return;
	}

	// lotem extracted as a member function in v1.2
	handle_nav_keys(ch, key_up, key_event);
}

//// lotem added in v1.2
void ComboFilter::handle_nav_keys( wchar_t ch, bool key_up, KeyEvent &key_event )
{
	wstring::size_type pos = nav_from_.find(ch);
	if (pos != wstring::npos) {
		if(!key_up)
		{
			send_vkcodes(wstring(1, nav_to_[pos]));
		}
	}
	key_event.Consumed(true);	
}

void ComboFilter::handle_enhanced_bksp_key( bool key_up, KeyEvent &key_event )
{
	if (!key_up)
	{					
		send_vkcodes(wstring(n_commited_, VK_BACK));
		reset_comb();
		n_commited_ = 1;
	}
	key_event.Consumed(true);
}

void ComboFilter::handle_comb_key( bool key_up, wstring::size_type pos, KeyEvent &key_event )
{
	if(key_up) 
	{
		if (comb_[pos])
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
				send_comb();
			}
			reset_comb();
		}
	}
	else // key down 
	{ 
		if (!comb_[pos])
		{
			comb_[pos] = true;
			++n_keys_held_;
			if(opt_repeat_)
			{
				timer_.Kill();
				timer_.Set(DELAY, boost::bind(&ComboFilter::on_timer, this));
			}
		}
	}
	key_event.Consumed(true);
}

void ComboFilter::send_comb()
{
	wstring text;
	for (wstring::size_type i = 0; i < comb_.size(); ++i)
	{
		if (comb_[i])
		{
			text += xlit_to_[i];
		}
	}
	if (!text.empty())
	{
		if (p_config_)
		{
			text = p_config_->ApplyRules(text);
		}
		
		if (text == L"~")
		{
			send_vkcodes(wstring(opt_enhanced_bksp_ ? n_commited_ : 1, VK_BACK));
			n_commited_ = 1;
		} 
		else 
		{
			send_vkcodes(text);
		}
	}
}

void ComboFilter::reset_comb()
{
	timer_.Kill();
	n_keys_held_ = 0;
	comb_.clear();
	comb_.resize(xlit_from_.length());
}

void ComboFilter::send_vkcodes( const wstring& vkcodes )
{
	if (vkcodes.empty())
	{
		return;
	}

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
		result.AddToTranslation(input);

		input.type = INPUT_KEYBOARD;
		input.ki.wVk = ch;
		input.ki.wScan = 0;
		input.ki.dwFlags = KEYEVENTF_KEYUP;
		input.ki.time = 0;
		input.ki.dwExtraInfo = 0;
		result.AddToTranslation(input);
	}

	Passage(result);
	n_commited_ = vkcodes.length();
}

void ComboFilter::Configure( const shared_ptr<ComboConfig>& p_config )
{
	xlit_from_ = p_config->XlitFrom();
	xlit_to_ = p_config->XlitTo();
	nav_from_ = p_config->NavFrom();
	nav_to_ = p_config->NavTo();
	p_config->SetOptRepeat(opt_repeat_);
	p_config->SetOptEnhancedBksp(opt_enhanced_bksp_);
	p_config->SetOptCtrlSpaceToggles(opt_ctrl_space_toggles_);	// lotem added in v1.2
	p_config->SetOptEnabled(enabled_);
	p_config_ = p_config;
	reset_comb();
}

void ComboFilter::Toggle()
{
	enabled_ = !enabled_;
	n_commited_ = 1;
	// alter display for IME state change
	Notify(TOGGLE, (UINT)enabled_);
}
