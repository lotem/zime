// Timer.h
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

#include <boost/function.hpp>
#include <boost/signal.hpp>

class Timer
{
public:
	virtual ~Timer();
	static Timer& create(HWND hWnd, UINT timer_id);

	void set( UINT delay, boost::function<void ()> func );
	void kill();
	UINT get_id() const	{ return timer_id_; }
private:
	Timer(HWND hWnd, UINT timer_id);

	HWND hWnd_;
	UINT timer_id_;

	static boost::signal<void ()> timer_event;

	friend VOID CALLBACK timer_proc(HWND hWnd, UINT message, UINT iTimerID, DWORD dwTime);
};
