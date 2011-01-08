// ComboPinyin.cpp
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
#include "ComboPinyin.h"
#include "ComboPinyinImpl.h"
#include <boost/bind.hpp>

void ComboPinyin::create(HINSTANCE hInstance, HWND hWnd)
{
	p_impl_ = new ComboPinyinImpl(hInstance, hWnd);
	p_impl_->connect( boost::bind(&Notifier::notify, this, _1, _2) );
}

void ComboPinyin::destroy()
{
	if (p_impl_)
	{
		delete p_impl_;
		p_impl_ = 0;
	}
}

ComboPinyin::~ComboPinyin()
{
	destroy();
}

void ComboPinyin::initialize()
{
	p_impl_->initialize();
}

void ComboPinyin::toggle()
{
	p_impl_->filter.toggle();
}

bool ComboPinyin::is_enabled() const
{
	return p_impl_->filter.is_enabled();
}

size_t ComboPinyin::get_config_count() const
{
	return p_impl_->config_browser.get_count();
}

const wstring& ComboPinyin::get_config_title( size_t n ) const
{
	return p_impl_->config_browser.get_title(n);
}

size_t ComboPinyin::get_active_config() const
{
	return p_impl_->active_config;
}

void ComboPinyin::configure( size_t n )
{
	p_impl_->configure(n);
}
