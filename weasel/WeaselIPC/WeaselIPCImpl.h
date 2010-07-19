#pragma once
#include <windows.h>
#include <WeaselIPC.h>

class WeaselIPC::WeaselIPCImpl
{
public:
	WeaselIPCImpl(void);
	~WeaselIPCImpl(void);

	void ConnectServer();
	void SendKey(UINT keyVal, LONG mask);
	UINT AddClient();
	void RemoveClient(UINT clientID);
	UINT EchoFromServer();

private:
	UINT clientID;
	HWND serverWnd;
	HANDLE sharedMem;
	TCHAR filemapName[MAX_PATH];
};
