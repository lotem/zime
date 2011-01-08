// ComboPinyinTrayIcon.h
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
#include "ComboPinyin.h"
#include "SystemTraySDK.h"

class ComboPinyinTrayIcon : public CSystemTray
{
public:
	ComboPinyinTrayIcon(ComboPinyin const& module)
		: module_(module) {}

protected:
	virtual void CustomizeMenu(HMENU hMenu);

private:
	ComboPinyin const& module_;
};
