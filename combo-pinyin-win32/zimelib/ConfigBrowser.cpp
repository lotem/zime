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

void ConfigBrowser::Browse()
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
				get_title(info);
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

size_t ConfigBrowser::Count() const
{
	return list_.size();
}

const wstring& ConfigBrowser::Title( size_t n ) const
{
	if (n >= list_.size())
	{
		return wstring();
	}
	return list_[n].title;
}

wstring const ConfigBrowser::FileName( size_t n ) const
{
	if (n >= list_.size())
	{
		return wstring();
	}
	return list_[n].file_name;
}

void ConfigBrowser::get_title( config_info& info )
{
	using namespace boost::algorithm;

	TextReader reader(info.file_name);
	if (!reader.IsOpen())
		return;

	info.title = fs::basename(info.file_name);

	wstring line;
	wstring directive;
	do
	{
		do 
		{
			directive = reader.GetDirective();

			if (starts_with(directive, L"@title "))
			{
				info.title = erase_head_copy(directive, 7);
			}

		} while(!directive.empty());

		line = reader.ReadLine();
		
	} while (!line.empty());

	reader.Close();
}
