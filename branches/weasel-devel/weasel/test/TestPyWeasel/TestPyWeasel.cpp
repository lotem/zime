// TestPyWeasel.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <boost/python.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <iostream>

#include <WeaselIPC.h>
#include <PyWeasel.h>

using namespace std;
namespace python = boost::python;

const char* wcstomb(const wchar_t* wcs)
{
	const int buffer_len = 8192;
	static char buffer[buffer_len];
	WideCharToMultiByte(CP_OEMCP, NULL, wcs, -1, buffer, buffer_len, NULL, FALSE);
	return buffer;
}

void test_pyweasel()
{
	weasel::RequestHandler* handler = new PyWeaselHandler();
	BOOST_ASSERT(handler);
	UINT sessionID = handler->AddSession();
	BOOST_ASSERT(sessionID);
	BOOST_ASSERT(sessionID == handler->FindSession(sessionID));
	WCHAR buffer[WEASEL_IPC_BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));
	BOOST_ASSERT(handler->ProcessKeyEvent(weasel::KeyEvent(L'a', 0), sessionID, buffer));
	cout << wcstomb(buffer) << endl;
	handler->RemoveSession(sessionID);
	delete handler;
}

int _tmain(int argc, _TCHAR* argv[])
{
  // Initialize the interpreter
  Py_Initialize();

  test_pyweasel();
  
  // Boost.Python doesn't support Py_Finalize yet, so don't call it!
  system("pause");
  return boost::report_errors();
}

