// KeyEvent.h
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

class KeyEvent 
{
public:
	typedef PKBDLLHOOKSTRUCT KeyInfoType;
	typedef INPUT InputType;
	typedef std::deque<INPUT> InputSequence;

	KeyEvent(KeyInfoType key_info = NULL);
	KeyInfoType get_key_info() const { return key_info_; }
	bool consumed() const { return consumed_; }
	void consumed(bool value) { consumed_ = value; }
	const InputSequence get_translation() const { return translation_; }
	void clear_translation() { translation_.clear(); }
	void add_translation( const InputType key );

private:
	const KeyInfoType key_info_;
	bool consumed_;
	InputSequence translation_;
};
