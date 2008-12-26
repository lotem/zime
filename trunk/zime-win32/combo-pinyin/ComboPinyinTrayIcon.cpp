// ComboPinyinTrayIcon.cpp
//
// ComboPinyin Source
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
#include "combo-pinyin.h"
#include "ComboPinyinTrayIcon.h"
#include "ComboPinyinModule.h"

extern ComboPinyinModule* pModule;

void ComboPinyinTrayIcon::CustomizeMenu( HMENU hMenu )
{
	HMENU hSubMenu = ::GetSubMenu(hMenu, 1);
	if (hSubMenu == NULL)
	{
		return;
	}
	size_t count = ::GetMenuItemCount(hSubMenu);
	while (count > 0)
	{
		::RemoveMenu(hSubMenu, (UINT) --count, MF_BYPOSITION);
	}

	if (!pModule)
	{
		return;
	}
	count = pModule->ConfigCount();
	if (count > MAX_LAYOUTS)
	{
		count = MAX_LAYOUTS;
	}

	for(size_t i = 0; i < count; ++i)
	{
		UINT flags = (i == pModule->ActiveConfig() ? MF_CHECKED : MF_UNCHECKED);
		::AppendMenu(hSubMenu, flags, IDM_LAYOUT + i, pModule->ConfigTitle(i).c_str());
	}
}
