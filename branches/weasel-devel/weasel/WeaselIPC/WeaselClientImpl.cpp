#include "stdafx.h"
#include "WeaselIPCImpl.h"

WeaselClient::Impl::Impl()
	: clientID(0),
	  serverWnd(NULL),
	  sharedMem(INVALID_HANDLE_VALUE)
{
	ConnectServer();
}

WeaselClient::Impl::~Impl()
{
}

void WeaselClient::Impl::ConnectServer()
{
	serverWnd = FindWindow( SERVER_WND_NAME, NULL );
	if( !serverWnd )
	{
		HANDLE hEvent = CreateEvent( NULL, TRUE, FALSE, SERVER_EVENT_NAME );
		HINSTANCE ret = ShellExecute( NULL, L"open", SERVER_EXEC, SERVER_ARGS, SERVER_DIR, SW_HIDE );
		WaitForSingleObject( hEvent, 10000 );
		CloseHandle(hEvent);
		serverWnd = FindWindow( SERVER_WND_NAME, NULL );
	}
}

void WeaselClient::Impl::ShutdownServer()
{
	SendMessage( serverWnd, WM_WEASEL_CMD_SHUTDOWN_SERVER, 0, 0 );
}

bool WeaselClient::Impl::ProcessKeyEvent(KeyEvent keyEvent)
{
	LRESULT ret = SendMessage( serverWnd, WM_WEASEL_CMD_PROCESS_KEY_EVENT, keyEvent, clientID );
	return ret != 0;
}

void WeaselClient::Impl::AddClient()
{
	clientID = SendMessage( serverWnd, WM_WEASEL_CMD_ADD_CLIENT, 0, 0 );	  
}

void WeaselClient::Impl::RemoveClient()
{
	SendMessage( serverWnd, WM_WEASEL_CMD_REMOVE_CLIENT, 0, clientID );
}

bool WeaselClient::Impl::EchoFromServer()
{
	UINT serverEcho = SendMessage( serverWnd, WM_WEASEL_CMD_ECHO, 0, clientID );
	return (serverEcho == clientID);
}
