#include "StdAfx.h"
#include "windows.h"
#include "shellapi.h"
#include "WeaselIPCImpl.h"

WeaselIPC::WeaselIPCImpl::WeaselIPCImpl()
	:clientID(0),
	 serverWnd(NULL),
	 sharedMem(INVALID_HANDLE_VALUE)
{
	ConnectServer();
}

WeaselIPC::WeaselIPCImpl::~WeaselIPCImpl()
{
}

void WeaselIPC::WeaselIPCImpl::ConnectServer()
{
	serverWnd = FindWindow( SERVER_WND_NAME, NULL );
	if( ! serverWnd )
	{
		HANDLE evt = CreateEvent( NULL, TRUE, FALSE, SERVER_EVENT_NAME );
		HINSTANCE ret = ShellExecute( NULL, L"open", L"D:\\WareHouse\\Workspace\\Weasel\\Debug\\WeaselServer.exe", NULL, NULL, SW_HIDE );
		WaitForSingleObject( evt, 10000 );
		CloseHandle(evt);
		serverWnd = FindWindow( SERVER_WND_NAME, NULL );
	}
}

void WeaselIPC::WeaselIPCImpl::CloseServer()
{
	SendMessage(serverWnd, WM_WEASEL_CMD_CLOSE_SERVER, 0, 0);
}

void WeaselIPC::WeaselIPCImpl::SendKey(UINT keyVal,  LONG mask)
{
	UINT keyEvent = 0x00000000;
	//	keyVal + mask -> keyEvent
	//后16位
	UINT keyVal32 = (UINT)keyVal & 0x0000ffff;
	//前16位
	UINT mask32 = (UINT)mask & 0x0000ffff;
	mask32 << 16;
	keyEvent =   mask32 |	keyVal32;

	SendMessage( serverWnd, WM_WEASEL_CMD_SEND_KEY, keyEvent, clientID );
}

void WeaselIPC::WeaselIPCImpl::AddClient()
{
	clientID  = SendMessage( serverWnd, WM_WEASEL_CMD_ADD_CLIENT, 0, 0 );	  
}

void WeaselIPC::WeaselIPCImpl::RemoveClient()
{
	SendMessage( serverWnd, WM_WEASEL_CMD_REMOVE_CLIENT, 0, clientID );
}

//以clientID请求server.若server端已注册则返回clientID
//否则返回-1
UINT WeaselIPC::WeaselIPCImpl::EchoFromServer()
{
	UINT serverEcho = SendMessage(serverWnd, WM_WEASEL_CMD_ECHO, 0, clientID);
	return serverEcho;
}