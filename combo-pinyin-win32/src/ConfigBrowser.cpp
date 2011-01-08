// ConfigBrowser.cpp
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
#include "ConfigBrowser.h"
#include "TextReader.h"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

wstring const ConfigBrowser::CONFIG_DIR = L"config";
wstring const ConfigBrowser::FILE_EXT = L".txt";
wstring const ConfigBrowser::AUTO_LOAD = L"autoload";

void ConfigBrowser::scan()
{
	list_.clear();

	fs::wpath path(CONFIG_DIR);
	if (!fs::exists(path) || !fs::is_directory(path))
	{
		return;
	}
	
	fs::wdirectory_iterator end_iter;
	for (fs::wdirectory_iterator iter(path); iter != end_iter; ++iter)
	{
		try
		{
			if (fs::is_regular(iter->status()) && 
				fs::extension(iter->path()) == FILE_EXT)
			{
				config_info info;
				info.file_name = iter->path().string();
				read_title(info);
				if (!info.title.empty())
				{
					if (fs::basename(iter->path()) == AUTO_LOAD)
					{
						list_.push_front(info);
					}
					else
					{
						list_.push_back(info);
					}
				}
			}
		}
		catch ( const std::exception & /*ex*/ )
		{
			// std::cerr << iter->path() << " " << ex.what() << std::endl;
		}
	}
}

size_t ConfigBrowser::get_count() const
{
	return list_.size();
}

wstring const& ConfigBrowser::get_title( size_t n ) const
{
	if (n >= list_.size())
	{
		return wstring();
	}
	return list_[n].title;
}

wstring const& ConfigBrowser::get_file_name( size_t n ) const
{
	if (n >= list_.size())
	{
		return wstring();
	}
	return list_[n].file_name;
}

void ConfigBrowser::read_title( config_info& info )
{
	using namespace boost::algorithm;

	TextReader reader(info.file_name);
	if (!reader.is_open())
		return;

	info.title = fs::basename(info.file_name);

	wstring line;
	wstring directive;
	do
	{
		do 
		{
			directive = reader.get_directive();

			if (starts_with(directive, L"@title "))
			{
				info.title = erase_head_copy(directive, 7);
			}

		} while(!directive.empty());

		line = reader.read_line();
		
	} while (!line.empty());

	reader.close();
}
