// TextReader.h
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

#include "AbstractReader.h"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;


class TextReader : AbstractReader
{
public:
	TextReader();
	TextReader(const fs::wpath file_name);
	virtual ~TextReader();

	void Open(const fs::wpath file_name);
	void Close();
	wstring const ReadLine() { return read_line_aux(); }
	wstring const GetDirective() { return read_line_aux(DIRECTIVE); }
	bool IsOpen() const;
	bool Eof() const;
	fs::wpath const& FileName() const { return file_name_; }
	const unsigned long GetLineNo() const { return line_no_; }

private:
	enum what_to_read
	{
		TEXT = 0,
		DIRECTIVE = 1
	};
	wstring const read_line_aux(what_to_read wanted = TEXT);
	std::ifstream stream_;
	unsigned long line_no_;
	wstring next_line_;
	fs::wpath file_name_;
};
