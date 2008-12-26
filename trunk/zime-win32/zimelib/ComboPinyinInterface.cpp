// ComboPinyinInterface.cpp
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
#include "ComboPinyinInterface.h"
#include "ComboPinyinImpl.h"
#include <boost/bind.hpp>

ComboPinyinInterface::ComboPinyinInterface(HINSTANCE hInstance, HWND hWnd)
{
	p_impl_ = new ComboPinyinImpl(hInstance, hWnd);
}

ComboPinyinInterface::~ComboPinyinInterface(void)
{
	if (p_impl_)
	{
		delete p_impl_;
		p_impl_ = 0;
	}
}

void ComboPinyinInterface::Initialize()
{
	p_impl_->RegisterEventHandler(
		boost::bind(&ComboPinyinInterface::OnNotify, this, _1, _2) );
	p_impl_->Initialize();
}

void ComboPinyinInterface::Toggle()
{
	p_impl_->Toggle();
}

bool ComboPinyinInterface::Enabled() const
{
	return p_impl_->Enabled();
}

size_t ComboPinyinInterface::ConfigCount() const
{
	return p_impl_->ConfigCount();
}

const wstring& ComboPinyinInterface::ConfigTitle( size_t n ) const
{
	return p_impl_->ConfigTitle(n);
}

void ComboPinyinInterface::Configure( size_t n )
{
	p_impl_->Configure(n);
}
