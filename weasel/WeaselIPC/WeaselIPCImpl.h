#pragma once
#include <windows.h>
#include <WeaselIPC.h>

class WeaselIPC::WeaselIPCImpl
{
public:
	WeaselIPCImpl(void);
	~WeaselIPCImpl(void);

	void ConnectServer();
	void CloseServer();
	void SendKey(UINT keyVal, LONG mask);
	void AddClient();
	void RemoveClient();
	UINT EchoFromServer();

private:
	UINT clientID;
	HWND serverWnd;
	HANDLE sharedMem;
	TCHAR filemapName[MAX_PATH];
};
