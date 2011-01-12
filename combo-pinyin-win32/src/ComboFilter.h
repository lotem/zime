// ComboFilter.h
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
#include "ComboConfig.h"
#include "Timer.h"
#include "Notifier.h"

class ComboFilter :	public KeyEventFilter, public Notifier
{
public:
	ComboFilter(Timer& timer);
	virtual ~ComboFilter();

	virtual void on_key_event(KeyEvent& key_event);

	void configure(const shared_ptr<ComboConfig>& p_config);

	void toggle();
	bool is_enabled() const { return enabled_; }

private:
	static const int DELAY = 200;

	enum key_state
	{
		RELEASED = 0,
		KEY_DOWN = 1,
		COMBINED = 2
	};
	key_state shift_state_;
	key_state super_state_;
	key_state space_state_;

	Timer& timer_;

	bool enabled_;

	shared_ptr<ComboConfig> p_config_;

	std::vector<bool> combo_;
	bool repeating_;
	int n_keys_held_;
	size_t n_commited_;

	void on_timer();

	void handle_shift( wchar_t ch, bool key_up );
	void handle_ctrl_space( bool key_up );
	void handle_nav_key( wchar_t ch, bool key_up, KeyEvent &key_event );
	void handle_caps( bool key_up, KeyEvent &key_event );
	void handle_super_combo( wchar_t ch, bool key_up, KeyEvent &key_event );
	void handle_enhanced_bksp( bool key_up, KeyEvent &key_event );
	void handle_combo_key( bool key_up, wstring::size_type pos, KeyEvent &key_event );

	bool is_chinese_ime_active() const;
	bool is_key_down(UINT vk) const;

	void send_vkcodes(const wstring& vkcodes);
	void send_combo();
	void reset_combo();
};
