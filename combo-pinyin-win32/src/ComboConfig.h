// ComboConfig.h
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

#include "RuleSet.h"
#include "StringTransformRule.h"
#include "ImportReader.h"

class ComboConfig
{
public:
	ComboConfig();
	void load(const fs::wpath file_name);
	const wstring apply_rules( const wstring& text );

	wstring xlit_from;
	wstring xlit_to;
	wstring nav_from;
	const wstring nav_to;
	wstring title;
	bool opt_enabled;
	bool opt_ctrl_space_switch;
	bool opt_enhanced_bksp;
	bool opt_repeat;

private:
	void process_directives( ImportReader& reader );
	bool set_opt_on_off( bool& opt, 
		                 const wstring& directive, 
						 const wstring& on, 
						 const wstring& off );
	void decode_xlit_string(wstring& xlit);

	RuleSet<StringTransformRule> rules_;
};
