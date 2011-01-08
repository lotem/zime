// Encoding.cpp
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
#include "Encoding.h"


const wstring Encoding::to_wide( const string& str )
{
	size_t max_len = str.length() + 1;
	wchar_t* p_wstr = new wchar_t[max_len];
	if(!MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, p_wstr, (int)max_len))
	{
		return wstring();
	}
	wstring ret(p_wstr);
	delete[] p_wstr;
	return ret;
}

const string Encoding::to_utf8( const wstring& wstr )
{
	size_t max_len = wstr.length() * 3 + 1;
	char* p_str = new char[max_len];
	if(!WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, p_str, (int)max_len, 0, 0))
	{
		return string();
	}
	string ret(p_str);
	delete[] p_str;
	return ret;
}

void Encoding::skip_bom( std::ifstream& ifs )
{
	if (!ifs.is_open())
	{
		return;
	}
	static const char BOM[3] = {(char)0xEF, (char)0xBB, (char)0xBF};
	char lc[3];
	int n;
	for(n = 0; n < 3 && !ifs.eof(); ++n)
		ifs.get(lc[n]);
	if(n < 3 || lc[0] != BOM[0] || lc[1] != BOM[1] || lc[2] != BOM[2]) {
		// BOM mismatch
		for(int i = 0; i < n; ++i)
			ifs.unget();
	}
}
