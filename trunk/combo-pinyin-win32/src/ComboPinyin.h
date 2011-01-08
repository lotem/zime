// ComboPinyin.h
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
#include "Notifier.h"

class ComboPinyinImpl;

class ComboPinyin : public Notifier
{
public:
	ComboPinyin() : p_impl_(0) {}
	virtual ~ComboPinyin();

	void create(HINSTANCE hInstance, HWND hWnd);
	void destroy();

	void initialize();

	void toggle();
	bool is_enabled() const;

	void configure(size_t n);
	size_t get_active_config() const;

	size_t get_config_count() const;
	wstring const& get_config_title(size_t n) const;

private:
	ComboPinyinImpl* p_impl_;
};
