// AbstractReader.h
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

class AbstractReader
{
public:
	virtual ~AbstractReader() {}
	virtual void close() = 0;
	virtual const wstring read_line() = 0;
	virtual const wstring get_directive() = 0;

	virtual bool is_open() const = 0;
	virtual bool eof() const = 0;

	static const wstring END_OF_FILE;
};