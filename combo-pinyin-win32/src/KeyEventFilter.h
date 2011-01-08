// KeyEventFilter.h
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

#include "KeyEvent.h"
#include <boost/signal.hpp>
#include <boost/signals/connection.hpp>


class KeyEventFilter
{
public:
	typedef boost::signal< void (KeyEvent&) > SignalType;

	virtual ~KeyEventFilter();
	virtual void on_key_event(KeyEvent&) = 0;
	void attach_to(KeyEventFilter& filter);
	void dettach();

protected:
	static SignalType sink;
	SignalType next;

private:
	boost::signals::connection conn_;
};
