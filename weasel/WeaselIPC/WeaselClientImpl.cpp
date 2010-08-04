#include "stdafx.h"
#include "WeaselIPCImpl.h"

WeaselClient::Impl::Impl()
	: clientID(0),
	  serverWnd(NULL)
{
}

WeaselClient::Impl::~Impl()
{
}

bool WeaselClient::Impl::ConnectServer(ServerLauncher launcher)
{
	serverWnd = FindWindow( SERVER_WND_NAME, NULL );
	if( !serverWnd && !launcher.empty() )
	{
		HANDLE hEvent = CreateEvent( NULL, TRUE, FALSE, SERVER_EVENT_NAME );
		// 启动服务进程
		if (!launcher())
		{
			CloseHandle(hEvent);
			serverWnd = NULL;
			return false;
		}
		WaitForSingleObject( hEvent, 10000 );
		CloseHandle(hEvent);
		serverWnd = FindWindow( SERVER_WND_NAME, NULL );
	}
	return (serverWnd != NULL);
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
	LRESULT ret = SendMessage( serverWnd, WM_WEASEL_CMD_ADD_CLIENT, 0, 0 );
	if (ret > 0)
	{
		clientID = ret;
	}
}

void WeaselClient::Impl::RemoveClient()
{
	SendMessage( serverWnd, WM_WEASEL_CMD_REMOVE_CLIENT, 0, clientID );
}

bool WeaselClient::Impl::EchoFromServer()
{
	if (clientID == 0)
	{
		return false;
	}
	UINT serverEcho = SendMessage( serverWnd, WM_WEASEL_CMD_ECHO, 0, clientID );
	return (serverEcho == clientID);
}

bool WeaselClient::Impl::GetResponseData(WeaselClient::ResponseHandler handler)
{
	try
	{
		windows_shared_memory shm(open_only, SHARED_MEMORY_NAME, read_only);
		mapped_region region(shm, read_only);

		return handler((LPWSTR)region.get_address(), DATA_BUFFER_SIZE / sizeof(WCHAR));
	}
	catch (interprocess_exception& /*ex*/)
	{
		return false;
	}

	return false;
}