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
	virtual void Close() = 0;
	virtual const wstring ReadLine() = 0;
	virtual const wstring GetDirective() = 0;

	virtual bool IsOpen() const = 0;
	virtual bool Eof() const = 0;

	static const wstring END_OF_FILE;
};