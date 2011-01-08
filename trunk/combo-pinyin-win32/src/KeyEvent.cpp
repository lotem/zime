// KeyEvent.cpp
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
#include "KeyEvent.h"

KeyEvent::KeyEvent(KeyEvent::KeyInfoType key_info) 
	: key_info_(key_info), consumed_(false), translation_() 
{
}
		
void KeyEvent::add_translation( const KeyEvent::InputType key )
{
	translation_.push_back( key );
}
