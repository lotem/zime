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
	const wstring& Title(size_t n) const;
	const string FileName(size_t n) const;

private:
	static char const * const CONFIG_DIR;
	static char const * const FILE_EXT;
	static char const * const AUTO_LOAD;
	struct config_info
	{
		wstring title;
		string file_name;
	};
	std::deque<config_info> list_;
	void get_title( config_info& info );
};
