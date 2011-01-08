// Timer.cpp
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
#include "Timer.h"

boost::signal<void ()> Timer::timer_event;

Timer::Timer(HWND hWnd, UINT timer_id)
:hWnd_(hWnd), timer_id_(timer_id)
{
}

Timer::~Timer()
{
	kill();
}

VOID CALLBACK timer_proc(HWND hWnd, UINT message, UINT iTimerID, DWORD dwTime)
{
	Timer::timer_event();
}

void Timer::set( UINT delay, boost::function<void ()> func )
{
	timer_event.connect(func);
	::SetTimer(hWnd_, timer_id_, delay, timer_proc);
}

void Timer::kill()
{
	::KillTimer(hWnd_, timer_id_);
	timer_event.disconnect_all_slots();
}

Timer& Timer::create( HWND hWnd, UINT timer_id )
{
	static Timer _timer(hWnd, timer_id);
	return _timer;
}
