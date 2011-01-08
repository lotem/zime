// ImportReader.h
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
#include "TextReader.h"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;


class ImportReader : public AbstractReader
{
public:
	ImportReader(const fs::wpath file_name);
	virtual ~ImportReader();	
	void import(const fs::wpath file_name) { open(file_name); }
	void close();
	const wstring read_line();
	const wstring get_directive();
	bool is_open() const;
	bool eof() const;
	bool is_imported() const; 
	const fs::wpath get_file_name() const;

protected:
	void open(const fs::wpath file_name);

private:
	typedef shared_ptr<TextReader> reader_handle;
	std::stack<reader_handle> readers_;
};
