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
	Open(file_name);
}

ImportReader::~ImportReader()
{
	Close();
}

void ImportReader::Close()
{
	while (!readers_.empty())
	{
		readers_.top()->Close();
		readers_.pop();
	}
}

const wstring ImportReader::ReadLine()
{
	wstring line;
	while (!readers_.empty())
	{
		line = readers_.top()->ReadLine();
		if (!line.empty())
		{
			break;
		}
		readers_.top()->Close();
		readers_.pop();
	}
	return line;
}

const wstring ImportReader::GetDirective()
{
	wstring directive;
	while (!readers_.empty())
	{
		directive = readers_.top()->GetDirective();
		if (!directive.empty() || !readers_.top()->Eof())
		{
			break;
		}
		readers_.top()->Close();
		readers_.pop();
	}
	return directive;
}

void ImportReader::Open( const fs::wpath file_name )
{
	reader_handle new_reader(new TextReader(file_name));
	if (new_reader->IsOpen())
	{
		readers_.push(new_reader);
	}
}

bool ImportReader::IsOpen() const
{
	return !readers_.empty();
}

bool ImportReader::Eof() const
{
	return readers_.empty();
}

bool ImportReader::IsImported() const
{
	return readers_.size() > 1;
}

const fs::wpath ImportReader::FileName() const
{
	return readers_.empty() ? fs::wpath() : readers_.top()->FileName();
}
