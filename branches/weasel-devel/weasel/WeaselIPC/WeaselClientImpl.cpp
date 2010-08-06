#include "stdafx.h"
#include "WeaselIPCImpl.h"

WeaselClient::Impl::Impl()
	: clientID(0),
	  serverWnd(NULL)
{
}

WeaselClient::Impl::~Impl()
{
	if (_Connected())
		DisconnectServer();
}

bool WeaselClient::Impl::ConnectServer(ServerLauncher const& launcher)
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
	return _Connected();
}

void WeaselClient::Impl::DisconnectServer()
{
	if (_Active())
		RemoveClient();
	serverWnd = NULL;
}

void WeaselClient::Impl::ShutdownServer()
{
	if (_Connected())
	{
		SendMessage( serverWnd, WM_WEASEL_CMD_SHUTDOWN_SERVER, 0, 0 );
	}
}

bool WeaselClient::Impl::ProcessKeyEvent(KeyEvent keyEvent)
{
	if (!_Active())
		return false;

	LRESULT ret = SendMessage( serverWnd, WM_WEASEL_CMD_PROCESS_KEY_EVENT, keyEvent, clientID );
	return ret != 0;
}

void WeaselClient::Impl::AddClient()
{
	if (!_Connected())
		return;

	if (_Active())
		RemoveClient();

	UINT ret = SendMessage( serverWnd, WM_WEASEL_CMD_ADD_CLIENT, 0, 0 );
	clientID = ret;
}

void WeaselClient::Impl::RemoveClient()
{
	if (_Connected())
		PostMessage( serverWnd, WM_WEASEL_CMD_REMOVE_CLIENT, 0, clientID );
	clientID = 0;
}

bool WeaselClient::Impl::EchoFromServer()
{
	if (!_Active())
		return false;

	UINT serverEcho = SendMessage( serverWnd, WM_WEASEL_CMD_ECHO, 0, clientID );
	return (serverEcho == clientID);
}

bool WeaselClient::Impl::GetResponseData(WeaselClient::ResponseHandler const& handler)
{
	if (handler.empty())
	{
		return false;
	}

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