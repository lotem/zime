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
	void Load(const fs::path file_name);
	const wstring ApplyRules( const wstring& text );

	wstring XlitFrom() const { return xlit_from_; }
	wstring XlitTo() const { return xlit_to_; }
	wstring NavFrom() const { return nav_from_; }
	wstring NavTo() const { return nav_to_; }
	void SetOptEnabled(bool& opt_) const 
	{ if (opt_enabled_set_) opt_ = opt_enabled_; }
	void SetOptRepeat(bool& opt_) const 
	{ if (opt_repeat_set_) opt_ = opt_repeat_; }
	void SetOptEnhancedBksp(bool& opt_) const 
	{ if (opt_enhanced_bksp_set_) opt_ = opt_enhanced_bksp_; }
	// lotem added in v1.2
	void SetOptCtrlSpaceToggles(bool& opt_) const 
	{ if (opt_ctrl_space_toggles_set_) opt_ = opt_ctrl_space_toggles_; }

	wstring Title() const { return title_; }

private:
	void process_directives( ImportReader& reader );
	bool set_opt_on_off( 
		bool& opt, 
		const wstring& directive, 
		const wstring& on, 
		const wstring& off );
	void decode_xlit_string(wstring& xlit);
	RuleSet<StringTransformRule> rules_;
	wstring xlit_from_;
	wstring xlit_to_;
	wstring nav_from_;
	const wstring nav_to_;
	wstring title_;
	bool opt_enabled_;
	bool opt_enabled_set_;
	bool opt_repeat_;
	bool opt_repeat_set_;
	bool opt_enhanced_bksp_;
	bool opt_enhanced_bksp_set_;
	// lotem added in v1.2
	bool opt_ctrl_space_toggles_;
	bool opt_ctrl_space_toggles_set_;
};
