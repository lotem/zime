#pragma once
#include <WeaselCommon.h>
#include <Windows.h>

enum WEASEL_CMD
{	
	WEASEL_CMD_ECHO = (WM_APP + 1),
	WEASEL_CMD_ADD_CLIENT,
	WEASEL_CMD_REMOVE_CLIENT,
	WEASEL_CMD_SEND_KEY	
};

class WeaselIPC
{
public:
	WeaselIPC();
	~WeaselIPC();

	void ConnectServer();
	void SendKey(UINT keyVal, LONG mask);
	UINT AddClient();
	void RemoveClient(UINT clientID);
	UINT EchoFromServer();

	UCHAR* GetDataFromSharedMem();

private:
	class WeaselIPCImpl;
	WeaselIPCImpl* m_pImpl;

};