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
#include "Encoding.h"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

using namespace boost::algorithm;


ComboConfig::ComboConfig() 
: nav_to_(L"\x26\x28\x25\x27\x24\x23\x21\x22\x08\x2e\x2d")
{
}

void ComboConfig::Load( const fs::path file_name )
{
	ImportReader reader(file_name);
	if (!reader.IsOpen())
		return;

	title_ = Encoding::ToWide(fs::basename(file_name));
	opt_enabled_ = false;
	opt_enabled_set_ = false;
	opt_repeat_ = false;
	opt_repeat_set_ = false;
	opt_enhanced_bksp_ = false;
	opt_enhanced_bksp_set_ = false;
	opt_ctrl_space_toggles_ = false;	// lotem added in v1.2
	opt_ctrl_space_toggles_set_ = false;	// lotem added in v1.2

	process_directives(reader);

	nav_from_ = reader.ReadLine();
	nav_from_.resize(nav_to_.length(), 0);

	xlit_from_ = reader.ReadLine();
	decode_xlit_string(xlit_from_);

	xlit_to_ = reader.ReadLine();
	xlit_to_.resize(xlit_from_.length(), 0);

	rules_.Load(&reader);

	process_directives(reader);

	reader.Close();
}

void ComboConfig::process_directives( ImportReader &reader )
{
	wstring directive;
	while (!( directive = reader.GetDirective() ).empty()) 
	{
		if (starts_with(directive, L"@import "))
		{
			fs::path import_file_name(reader.FileName());
			import_file_name.remove_leaf();
			import_file_name /=	Encoding::ToUTF8(erase_head_copy(directive, 8));
			reader.Import(import_file_name);
			continue;
		}
		if (!reader.IsImported() && starts_with(directive, L"@title "))
		{
			title_ = erase_head_copy(directive, 7);
			continue;
		}
		set_opt_on_off(opt_enabled_, directive, L"@enable", L"@disable") && 
			(opt_enabled_set_ = true) ||
		set_opt_on_off(opt_repeat_, directive, L"@repeat on", L"@repeat off") && 
			(opt_repeat_set_ = true) ||
		set_opt_on_off(opt_enhanced_bksp_, directive, L"@enhanced_bksp on", L"@enhanced_bksp off") && 
			(opt_enhanced_bksp_set_ = true) ||
		set_opt_on_off(opt_ctrl_space_toggles_, directive, L"@ctrl_space_toggles on", L"@ctrl_space_toggles off") && 
			(opt_ctrl_space_toggles_set_ = true);	// lotem appended in v1.2
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
	// TODO: represent all OEM keys
}

const wstring ComboConfig::ApplyRules( const wstring& text )
{
	return rules_.Apply(text);
}
