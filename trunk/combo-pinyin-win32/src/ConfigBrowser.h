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
	void scan();
	size_t get_count() const;
	wstring const& get_title(size_t n) const;
	wstring const& get_file_name(size_t n) const;

protected:
	static wstring const CONFIG_DIR;
	static wstring const FILE_EXT;
	static wstring const AUTO_LOAD;

private:
	struct config_info
	{
		wstring title;
		wstring file_name;
	};
	void read_title( config_info& info );
	std::deque<config_info> list_;
};
