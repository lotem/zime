// Notifier.h
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


class Notifier
{
public:
	typedef boost::function<void (UINT, UINT)>  EventHandler;

	void RegisterEventHandler(EventHandler event_handler)
	{
		notifier_.connect(event_handler);
	}

	void Notify(UINT event_id, UINT param)
	{
		notifier_(event_id, param);
	}

private:
	boost::signal<void (UINT, UINT)> notifier_;
};