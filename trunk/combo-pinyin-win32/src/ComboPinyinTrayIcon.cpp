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
#include "ComboPinyinApp.h"
#include "ComboPinyinTrayIcon.h"

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

	count = module_.get_config_count();
	if (count > MAX_LAYOUTS)
	{
		count = MAX_LAYOUTS;
	}

	for(size_t i = 0; i < count; ++i)
	{
		UINT flags = (i == module_.get_active_config() ? MF_CHECKED : MF_UNCHECKED);
		::AppendMenu(hSubMenu, flags, IDM_LAYOUT + i, module_.get_config_title(i).c_str());
	}
}
