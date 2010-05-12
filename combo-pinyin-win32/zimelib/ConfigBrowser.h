// ConfigBrowser.h
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

class ConfigBrowser
{
public:
	void Browse();
	size_t Count() const;
	wstring const& Title(size_t n) const;
	wstring const FileName(size_t n) const;

private:
	static wstring const CONFIG_DIR;
	static wstring const FILE_EXT;
	static wstring const AUTO_LOAD;
	struct config_info
	{
		wstring title;
		wstring file_name;
	};
	std::deque<config_info> list_;
	void get_title( config_info& info );
};
