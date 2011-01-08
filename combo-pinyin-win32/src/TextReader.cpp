// TextReader.cpp
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
#include "TextReader.h"
#include "Encoding.h"

using namespace boost::algorithm;

TextReader::TextReader() 
: stream_(), line_no_(0), next_line_()
{
}

TextReader::TextReader( const fs::wpath file_name )
{
	open(file_name);
}

TextReader::~TextReader()
{
	close();
}


const wstring TextReader::read_line_aux( what_to_read wanted )
{
	if (!is_open())
	{
		return END_OF_FILE;
	}
	
	if (!next_line_.empty())
	{
		if (wanted == DIRECTIVE)
		{
			return END_OF_FILE;
		} 
		else // wants TEXT
		{
			wstring line = next_line_;
			next_line_.clear();
			return line;
		}
	}

	string line;
	do {
		if(stream_.eof())
		{
			return END_OF_FILE;
		}

		getline(stream_, line);
		++line_no_;

		// a "#@directive" line
		if (wanted == DIRECTIVE && starts_with(line, "#@"))
		{
			trim_right(line);
			trim_if(line, is_any_of(L"#@ \t"));
			line.insert(0, 1, L'@');
			return Encoding::to_wide(line);
		}

		// remove comments
		string::size_type pos = line.find('#');
		if (pos != string::npos)
		{
			line.erase(pos);
		}
		
		trim_right(line);
		
	} while(line.empty());	// loop until we met a useful line

	if (wanted == DIRECTIVE)
	{
		next_line_ = Encoding::to_wide(line);
		return END_OF_FILE;
	} 
	else // wants TEXT
	{
		return Encoding::to_wide(line);
	}
}

void TextReader::open( const fs::wpath file_name )
{
	close();
	stream_.open(fs::system_complete(file_name).string().c_str());
	file_name_ = file_name;
	Encoding::skip_bom(stream_);
}

void TextReader::close()
{
	line_no_ = 0;
	next_line_.clear();
	if (stream_.is_open())
	{
		stream_.close();
	}
}

bool TextReader::is_open() const
{
	return stream_.is_open();
}

bool TextReader::eof() const
{
	return next_line_.empty() && stream_.eof();
}
