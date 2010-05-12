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
	void Import(const fs::wpath file_name) { Open(file_name); }
	void Close();
	const wstring ReadLine();
	const wstring GetDirective();
	bool IsOpen() const;
	bool Eof() const;
	bool IsImported() const; 
	const fs::wpath FileName() const;

private:
	void Open(const fs::wpath file_name);
	typedef shared_ptr<TextReader> reader_handle;
	std::stack<reader_handle> readers_;
};
