// KeyEventFilter.cpp
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
#include "KeyEventFilter.h"
#include <boost/bind.hpp>

KeyEventFilter::SignalType KeyEventFilter::sink;

KeyEventFilter::~KeyEventFilter()
{
	dettach();
}

void KeyEventFilter::attach_to( KeyEventFilter& filter )
{
	conn_ = filter.next.connect(boost::bind(&KeyEventFilter::on_key_event, this, _1));
}

void KeyEventFilter::dettach()
{
	conn_.disconnect();
}
