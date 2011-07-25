// TestPyWeasel.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <boost/detail/lightweight_test.hpp>
#include <iostream>

#include <WeaselIPC.h>
#include <PyWeasel.h>

using namespace std;

const char* wcstomb(const wchar_t* wcs)
{
	const int buffer_len = 8192;
	static char buffer[buffer_len];
	WideCharToMultiByte(CP_OEMCP, NULL, wcs, -1, buffer, buffer_len, NULL, FALSE);
	return buffer;
}

void test_pyweasel()
{
	WCHAR buffer[WEASEL_IPC_BUFFER_LENGTH];

	weasel::RequestHandler* handler = new PyWeaselHandler;
	BOOST_ASSERT(handler);

	handler->Initialize();

	memset(buffer, 0, sizeof(buffer));
	// �ɹ������Ự������sessionID��ʧ�ܷ���0
	UINT sessionID = handler->AddSession(buffer);
	BOOST_ASSERT(sessionID);
	cout << wcstomb(buffer) << endl;

	// �Ự���ڷ���sessionID�������ڷ���0
	BOOST_ASSERT(sessionID == handler->FindSession(sessionID));

	memset(buffer, 0, sizeof(buffer));
	// ���� a��ProcessKeyEventӦ����TRUE��������Ӧ��д��buffer
	BOOL taken = handler->ProcessKeyEvent(weasel::KeyEvent(L'a', 0), sessionID, buffer);
	BOOST_ASSERT(taken);
	// Windows����̨����ֱ����ʾWideChar���Ĵ���תΪMultiByteString
	cout << wcstomb(buffer) << endl;
	
	// �ɹ����ٻỰ������sessionID��ʧ�ܷ���0
	BOOST_ASSERT(sessionID == handler->RemoveSession(sessionID));

	handler->Finalize();

	delete handler;
}

int _tmain(int argc, _TCHAR* argv[])
{
  test_pyweasel();
  
  system("pause");
  return boost::report_errors();
}

