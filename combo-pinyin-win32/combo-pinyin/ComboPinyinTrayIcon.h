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
#include "SystemTraySDK.h"

class ComboPinyinTrayIcon : public CSystemTray
{
protected:
	virtual void CustomizeMenu(HMENU hMenu);
};

