// Encoding.h
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

class Encoding
{
public:
	static const wstring to_wide(const string& str);
	static const string to_utf8(const wstring& wstr);
	static void skip_bom( std::ifstream& ifs );
private:
	Encoding();
};
