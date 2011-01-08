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

#include <boost/signals.hpp>

class Notifier : public boost::signal<void (UINT, UINT)>
{
public:
	typedef slot_type EventHandler;

	void notify(UINT event_id, UINT param)
	{
		(*this)(event_id, param);
	}

};