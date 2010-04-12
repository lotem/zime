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
	KeyInfoType KeyInfo() const { return key_info_; }
	bool Consumed() const { return consumed_; }
	void Consumed(bool value) { consumed_ = value; }
	bool PassOn() const { return pass_on_; }
	void PassOn(bool value) { pass_on_ = value; }
	const InputSequence Translation() const { return translation_; }
	void ClearTranslation() { translation_.clear(); }
	void AddToTranslation( const InputType key );

private:
	const KeyInfoType key_info_;
	bool consumed_;
	bool pass_on_;
	InputSequence translation_;
};
