// ImportReader.cpp
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
#include "ImportReader.h"

ImportReader::ImportReader(const fs::wpath file_name)
{
	open(file_name);
}

ImportReader::~ImportReader()
{
	close();
}

void ImportReader::close()
{
	while (!readers_.empty())
	{
		readers_.top()->close();
		readers_.pop();
	}
}

const wstring ImportReader::read_line()
{
	wstring line;
	while (!readers_.empty())
	{
		line = readers_.top()->read_line();
		if (!line.empty())
		{
			break;
		}
		readers_.top()->close();
		readers_.pop();
	}
	return line;
}

const wstring ImportReader::get_directive()
{
	wstring directive;
	while (!readers_.empty())
	{
		directive = readers_.top()->get_directive();
		if (!directive.empty() || !readers_.top()->eof())
		{
			break;
		}
		readers_.top()->close();
		readers_.pop();
	}
	return directive;
}

void ImportReader::open( const fs::wpath file_name )
{
	reader_handle new_reader(new TextReader(file_name));
	if (new_reader->is_open())
	{
		readers_.push(new_reader);
	}
}

bool ImportReader::is_open() const
{
	return !readers_.empty();
}

bool ImportReader::eof() const
{
	return readers_.empty();
}

bool ImportReader::is_imported() const
{
	return readers_.size() > 1;
}

const fs::wpath ImportReader::get_file_name() const
{
	return readers_.empty() ? fs::wpath() : readers_.top()->get_file_name();
}
