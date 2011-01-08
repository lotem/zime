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
	virtual ~ComboPinyinImpl();

	void initialize();
	void tear_down();

	// override
	void connect( EventHandler event_handler );

	void configure(size_t n);

	ComboFilter filter;
	ConfigBrowser config_browser;
	size_t active_config;

private:
	enum
	{
		TIMER_ID = 1
	};
	bool setup_;
	HINSTANCE hInstance_;
};
