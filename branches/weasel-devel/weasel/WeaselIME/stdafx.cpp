// stdafx.cpp : source file that includes just the standard includes
// WeaselIME.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

using std::wstring;
// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
bool read_buffer(LPWSTR buffer, UINT length, wstring& dest)
{
	wbufferstream bs(buffer, length);
	getline(bs, dest);
	return bs.good();
}

bool launch_server()
{
	int ret = (int)ShellExecute( NULL, L"open", L"TestWeaselIPC.exe", L"/start", NULL, SW_HIDE );
	if (ret <= 32)
	{
		cerr << "failed to launch server." << endl;
		return false;
	}
	return true;
}