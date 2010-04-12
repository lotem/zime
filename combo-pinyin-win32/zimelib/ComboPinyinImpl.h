// ComboPinyinImpl.h
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

#include "ComboFilter.h"
#include "ConfigBrowser.h"
#include "Notifier.h"

class ComboPinyinImpl : public Notifier
{
public:
	ComboPinyinImpl(HINSTANCE hInstance, HWND hWnd);
	virtual ~ComboPinyinImpl(void);
	void Initialize();
	void TearDown();
	void Toggle() { filter_.Toggle(); }
	bool Enabled() const { return filter_.Enabled(); }
	size_t ConfigCount() const { return config_browser_.Count(); }
	const wstring& ConfigTitle(size_t n) const { return config_browser_.Title(n); }
	void Configure(size_t n);
	// overrides Notifier member function
	void RegisterEventHandler( EventHandler event_handler );
private:
	enum
	{
		TIMER_ID = 1
	};
	bool setup_;
	HINSTANCE hInstance_;
	ComboFilter filter_;
	ConfigBrowser config_browser_;
};
