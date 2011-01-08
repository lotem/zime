// ComboPinyinImpl.cpp
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
#include "ComboPinyinImpl.h"
#include "EventDef.h"
#include "KeyboardHook.h"
#include "Timer.h"


ComboPinyinImpl::ComboPinyinImpl(HINSTANCE hInstance, HWND hWnd)
: setup_(false), hInstance_(hInstance), config_browser(), filter(Timer::create(hWnd, TIMER_ID))
{
}

ComboPinyinImpl::~ComboPinyinImpl(void)
{
	tear_down();
}

void ComboPinyinImpl::connect( EventHandler event_handler )
{
	Notifier::connect(event_handler);
	filter.connect(event_handler);
}

void ComboPinyinImpl::configure( size_t n )
{
	shared_ptr<ComboConfig> p_config(new ComboConfig());
	p_config->load(config_browser.get_file_name(n));
	filter.configure(p_config);
	active_config = n;
	notify(LAYOUT_CHANGE, (UINT)n);
}

void ComboPinyinImpl::initialize()
{
	if (setup_)
	{
		tear_down();
	}

	setup_ = true;

	config_browser.scan();

	if (config_browser.get_count() > 0)
	{
		configure(0);
	}

	KeyboardHook& hook = KeyboardHook::get_instance();
	filter.attach_to(hook);
	hook.install_hook(hInstance_);
}

void ComboPinyinImpl::tear_down()
{
	if (!setup_)
		return;

	KeyboardHook::get_instance().uninstall_hook();
	filter.dettach();

	setup_ = false;
}
