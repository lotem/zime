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

KeyEventFilter::SignalType KeyEventFilter::Sink;

KeyEventFilter::~KeyEventFilter()
{
	Dettach();
}

void KeyEventFilter::AttachTo( KeyEventFilter& filter )
{
	UpLink = filter.Passage.connect(boost::bind(&KeyEventFilter::OnKey, this, _1));
}

void KeyEventFilter::Dettach()
{
	UpLink.disconnect();
}
