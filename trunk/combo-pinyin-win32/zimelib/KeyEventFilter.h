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
	virtual void OnKey(KeyEvent&) = 0;
	void AttachTo(KeyEventFilter& filter);
	void Dettach();

protected:
	static SignalType Sink;
	SignalType Passage;
	boost::signals::connection UpLink;
};
