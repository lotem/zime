// ComboPinyinModule.h
//
// ComboPinyin Header
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

#include "../zimelib/ComboPinyinInterface.h"
#include "../zimelib/EventDef.h"


class ComboPinyinModule : public ComboPinyinInterface
{
public:
	typedef void (*EventHandler)(UINT, UINT);

	ComboPinyinModule(HINSTANCE hInstance, HWND hWnd, EventHandler event_handler)
		: ComboPinyinInterface(hInstance, hWnd), event_handler_(event_handler)
	{
	}
	virtual void OnNotify( UINT event_id, UINT param )
	{
		event_handler_(event_id, param);
	}
	size_t ActiveConfig() const { return active_config_; }
	void ActiveConfig(size_t n) { active_config_ = n; }

private:
	EventHandler event_handler_;
	size_t active_config_;
};
