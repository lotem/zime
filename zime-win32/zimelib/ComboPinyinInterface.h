// ComboPinyinInterface.h
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

class ComboPinyinImpl;

class ComboPinyinInterface
{
public:
	ComboPinyinInterface(HINSTANCE hInstance, HWND hWnd);
	virtual ~ComboPinyinInterface(void);

	void Initialize();
	virtual void OnNotify(UINT event_id, UINT param) = 0;
	void Toggle();
	bool Enabled() const;
	size_t ConfigCount() const;
	const wstring& ConfigTitle(size_t n) const;
	void Configure(size_t n);

private:
	ComboPinyinImpl* p_impl_;
};
