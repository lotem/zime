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
#include "Timer.h"
#include "KeyboardHook.h"
#include "EventDef.h"


ComboPinyinImpl::ComboPinyinImpl(HINSTANCE hInstance, HWND hWnd)
: setup_(false), hInstance_(hInstance), filter_(Timer::Create(hWnd, TIMER_ID))
{
}

ComboPinyinImpl::~ComboPinyinImpl(void)
{
	TearDown();
}

void ComboPinyinImpl::Configure( size_t n )
{
	shared_ptr<ComboConfig> p(new ComboConfig());
	p->Load(config_browser_.FileName(n));
	filter_.Configure(p);
	Notify(LAYOUT_CHANGE, (UINT)n);
}

void ComboPinyinImpl::RegisterEventHandler( EventHandler event_handler )
{
	Notifier::RegisterEventHandler(event_handler);
	filter_.RegisterEventHandler(event_handler);
}

void ComboPinyinImpl::Initialize()
{
	if (setup_)
	{
		TearDown();
	}

	setup_ = true;

	config_browser_.Browse();

	if (config_browser_.Count() > 0)
	{
		Configure(0);
	}

	filter_.AttachTo(KeyboardHook::Instance());
	KeyboardHook::Instance().InstallHook(hInstance_);
}

void ComboPinyinImpl::TearDown()
{
	if (!setup_)
	{
		return;
	}

	KeyboardHook::Instance().UninstallHook();
	filter_.Dettach();

	setup_ = false;
}
