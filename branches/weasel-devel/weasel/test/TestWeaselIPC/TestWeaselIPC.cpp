// TestWeaselIPC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <WeaselIPC.h>

#include <iostream>
using namespace std;

CAppModule _Module;

int client_main();
int server_main();

// usage: TestWeaselIPC.exe [/start | /stop]

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc == 1)  // no args
	{
		return client_main();
	}
	else if(argc > 1 && !wcscmp(L"/start", argv[1]))
	{
		return server_main();
	}
	else if (argc > 1 && !wcscmp(L"/stop", argv[1]))
	{
		WeaselClient client;
		if (!client.ConnectServer())
		{
			cerr << "server not running." << endl;
			return 0;
		}
		client.ShutdownServer();
		return 0;
	}

	return -1;
}

bool launch_server()
{
	int ret = (int)ShellExecute( NULL, L"open", L"TestWeaselIPC.exe", L"/start", NULL, SW_HIDE );
	if (ret <= 32)
	{
		cerr << "failed to launch server." << endl;
		return false;
	}
	system("pause");
	return true;
}

int client_main()
{
	WeaselClient client;
	if (!client.ConnectServer(launch_server))
	{
		cerr << "failed to connect to server." << endl;
		return -1;
	}
	client.AddClient();
	if (!client.EchoFromServer())
	{
		cerr << "failed to login." << endl;
		return -1;
	}
	bool eaten = client.ProcessKeyEvent(KeyEvent(L'A', 0));
	cout << "server replies: " << eaten << endl;
	client.RemoveClient();

	return 0;
}

class TestRequestHandler : public WeaselServer::RequestHandler
{
public:
	TestRequestHandler() : m_counter(0)
	{
		cerr << "handler ctor." << endl;
	}
	virtual ~TestRequestHandler()
	{
		cerr << "handler dtor: " << m_counter << endl;
	}
	virtual UINT FindClient(UINT clientID)
	{
		cerr << "FindClient: " << clientID << endl;
		return (clientID <= m_counter ? clientID : 0);
	}
	virtual UINT AddClient()
	{
		cerr << "AddClient: " << m_counter + 1 << endl;
		return ++m_counter;
	}
	virtual UINT RemoveClient(UINT clientID)
	{
		cerr << "RemoveClient: " << clientID << endl;
		return 0;
	}
	virtual BOOL ProcessKeyEvent(KeyEvent keyEvent, UINT clientID) {
		cerr << "ProcessKeyEvent: " << clientID << " keycode: " << keyEvent.keyCode << " mask:" << keyEvent.mask << endl;
		return TRUE;
	}
private:
	unsigned int m_counter;
};

int server_main()
{
	HRESULT hRes = _Module.Init(NULL, GetModuleHandle(NULL));
	ATLASSERT(SUCCEEDED(hRes));

	WeaselServer server(new TestRequestHandler());
	if (!server.StartServer())
		return -1;
	cerr << "server running." << endl;
	int ret = server.Run();
	cerr << "server quitting." << endl;
	return ret;
}
