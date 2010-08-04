// TestWeaselIPC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

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

int server_main()
{
	HRESULT hRes = _Module.Init(NULL, GetModuleHandle(NULL));
	ATLASSERT(SUCCEEDED(hRes));

	WeaselServer server;
	//server.RegisterRequestHandler(...);
	if (!server.StartServer())
		return -1;
	cerr << "server running." << endl;
	int ret = server.Run();
	cerr << "server quitting." << endl;
	return ret;
}
