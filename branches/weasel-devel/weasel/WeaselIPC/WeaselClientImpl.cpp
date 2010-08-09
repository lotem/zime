#include "stdafx.h"
#include "WeaselClientImpl.h"

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
	serverWnd = FindWindow( WEASEL_IPC_WINDOW, NULL );
	if( !serverWnd && !launcher.empty() )
	{
		HANDLE hEvent = CreateEvent( NULL, TRUE, FALSE, WEASEL_IPC_READY_EVENT );
		// 启动服务进程
		if (!launcher())
		{
			CloseHandle(hEvent);
			serverWnd = NULL;
			return false;
		}
		WaitForSingleObject( hEvent, 10000 );
		CloseHandle(hEvent);
		serverWnd = FindWindow( WEASEL_IPC_WINDOW, NULL );
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
		SendMessage( serverWnd, WEASEL_IPC_SHUTDOWN_SERVER, 0, 0 );
	}
}

bool WeaselClient::Impl::ProcessKeyEvent(KeyEvent keyEvent)
{
	if (!_Active())
		return false;

	LRESULT ret = SendMessage( serverWnd, WEASEL_IPC_PROCESS_KEY_EVENT, keyEvent, clientID );
	return ret != 0;
}

void WeaselClient::Impl::AddClient()
{
	if (!_Connected())
		return;

	if (_Active())
		RemoveClient();

	UINT ret = SendMessage( serverWnd, WEASEL_IPC_ADD_CLIENT, 0, 0 );
	clientID = ret;
}

void WeaselClient::Impl::RemoveClient()
{
	if (_Connected())
		PostMessage( serverWnd, WEASEL_IPC_REMOVE_CLIENT, 0, clientID );
	clientID = 0;
}

bool WeaselClient::Impl::EchoFromServer()
{
	if (!_Active())
		return false;

	UINT serverEcho = SendMessage( serverWnd, WEASEL_IPC_ECHO, 0, clientID );
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
		windows_shared_memory shm(open_only, WEASEL_IPC_SHARED_MEMORY, read_only);
		mapped_region region(shm, read_only);

		return handler((LPWSTR)region.get_address(), WEASEL_IPC_BUFFER_SIZE / sizeof(WCHAR));
	}
	catch (interprocess_exception& /*ex*/)
	{
		return false;
	}

	return false;
}

// WeaselClient

WeaselClient::WeaselClient() 
	: m_pImpl(new WeaselClient::Impl())
{}

WeaselClient::~WeaselClient()
{
	if (m_pImpl)
		delete m_pImpl;
}

bool WeaselClient::ConnectServer(ServerLauncher launcher)
{
	return m_pImpl->ConnectServer(launcher);
}

void WeaselClient::DisconnectServer()
{
	m_pImpl->DisconnectServer();
}

void WeaselClient::ShutdownServer()
{
	m_pImpl->ShutdownServer();
}

bool WeaselClient::ProcessKeyEvent(KeyEvent keyEvent)
{
	return m_pImpl->ProcessKeyEvent(keyEvent);
}

void WeaselClient::AddClient()
{
	m_pImpl->AddClient();
}

void WeaselClient::RemoveClient()
{
	m_pImpl->RemoveClient();
}

bool WeaselClient::EchoFromServer()
{
	return m_pImpl->EchoFromServer();
}

bool WeaselClient::GetResponseData(WeaselClient::ResponseHandler handler)
{
	return m_pImpl->GetResponseData(handler);
}
