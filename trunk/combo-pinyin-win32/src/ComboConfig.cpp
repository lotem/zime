// ComboConfig.cpp
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
#include "ComboConfig.h"
#include "ImportReader.h"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

using namespace boost::algorithm;


ComboConfig::ComboConfig() 
: nav_to(L"\x26\x28\x25\x27\x24\x23\x21\x22\x08\x2e\x2d\x09\x1b")
{
}

void ComboConfig::load( const fs::wpath file_name )
{
	ImportReader reader(file_name);
	if (!reader.is_open())
		return;

	title = fs::basename(file_name);
	opt_enabled = false;
	opt_repeat = false;
	opt_enhanced_bksp = false;
	opt_ctrl_space_switch = false;
	opt_caps_as_super = true;
	opt_lwin_as_super = true;

	process_directives(reader);

	nav_from = reader.read_line();
	nav_from.resize(nav_to.length(), 0);

	xlit_from = reader.read_line();
	decode_xlit_string(xlit_from);

	xlit_to = reader.read_line();
	xlit_to.resize(xlit_from.length(), 0);

	rules_.load(&reader);

	process_directives(reader);

	reader.close();
}

void ComboConfig::process_directives( ImportReader &reader )
{
	wstring directive;
	while (!( directive = reader.get_directive() ).empty()) 
	{
		if (starts_with(directive, L"@import "))
		{
			fs::wpath import_file_name(reader.get_file_name());
			import_file_name.remove_leaf();
			import_file_name /=	erase_head_copy(directive, 8);
			reader.import(import_file_name);
			continue;
		}
		if (!reader.is_imported() && starts_with(directive, L"@title "))
		{
			title = erase_head_copy(directive, 7);
			continue;
		}
		set_opt_on_off(opt_enabled, directive, L"@enable", L"@disable") ||
		set_opt_on_off(opt_repeat, directive, L"@repeat on", L"@repeat off") ||
		set_opt_on_off(opt_enhanced_bksp, directive, L"@enhanced_bksp on", L"@enhanced_bksp off") ||
		set_opt_on_off(opt_ctrl_space_switch, directive, L"@ctrl_space_switch on", L"@ctrl_space_switch off") ||
		set_opt_on_off(opt_caps_as_super, directive, L"@caps_as_super on", L"@caps_as_super off") ||
		set_opt_on_off(opt_lwin_as_super, directive, L"@lwin_as_super on", L"@lwin_as_super off");
	}
}

bool ComboConfig::set_opt_on_off( bool& opt, const wstring& directive, const wstring& on, const wstring& off )
{
	if (directive == on)
	{
		opt = true;
		return true; // set
	}
	else if (directive == off)
	{
		opt = false;
		return true; // set
	}
	return false; // not set
}

void ComboConfig::decode_xlit_string(wstring& xlit)
{
	replace_all(xlit, L"_", L" ");
	replace_all(xlit, L"<", L"\xbc");
	replace_all(xlit, L">", L"\xbe");
	replace_all(xlit, L";", L"\xba");
	replace_all(xlit, L"/", L"\xbf");
	replace_all(xlit, L"'", L"\xde");
	// TODO: other OEM keys
}

const wstring ComboConfig::apply_rules( const wstring& text )
{
	return rules_.apply(text);
}
