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
	virtual void OnKey(KeyEvent& key_event);

	void Configure(const shared_ptr<ComboConfig>& p_config);
	void Toggle();
	bool Enabled() const { return enabled_; }

private:
	static const int DELAY = 200;

	enum key_state
	{
		RELEASED = 0,
		HELD_DOWN = 1,
		COMBINED = 2
	};
	key_state shift_state_;
	key_state caps_state_;
	key_state space_state_;

	Timer& timer_;
	bool repeating_;
	bool enabled_;

	int n_keys_held_;
	std::vector<bool> comb_;
	size_t n_commited_;

	wstring xlit_from_;
	wstring xlit_to_;
	wstring nav_from_;
	wstring nav_to_;
	bool opt_repeat_;
	bool opt_enhanced_bksp_;
	bool opt_ctrl_space_toggles_;	// lotem added in v1.2
	shared_ptr<ComboConfig> p_config_;

	void handle_shift( wchar_t ch, bool key_up );
	void handle_ctrl_space( bool key_up );
	void handle_nav_keys( wchar_t ch, bool key_up, KeyEvent &key_event );	// lotem added in v1.2
	void handle_caps_key( bool key_up, KeyEvent &key_event );
	void handle_caps_comb( wchar_t ch, bool key_up, KeyEvent &key_event );
	void handle_enhanced_bksp_key( bool key_up, KeyEvent &key_event );
	void handle_comb_key( bool key_up, wstring::size_type pos, KeyEvent &key_event );
	bool is_key_down(UINT vk);
	void send_comb();
	void reset_comb();
	void send_vkcodes(const wstring& vkcodes);
	void on_timer();
};
