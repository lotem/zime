#include "stdafx.h"
#include "WeaselIPCImpl.h"
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

WeaselClient::Impl::Impl()
	: clientID(0),
	  serverWnd(NULL)
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
		{
			// 启动服务进程
			// TODO: 暂写定一个路径 应改为从注册表读取安装目录
			int ret = (int)ShellExecute( NULL, L"open", SERVER_EXEC, SERVER_ARGS, SERVER_DIR, SW_HIDE );
			if (ret <= 32)
			{
				MessageBox(NULL, L"服務進程啓動不起來:(", L"小狼毫", MB_OK | MB_ICONERROR);
				CloseHandle(hEvent);
				serverWnd = NULL;
				return;
			}
		}
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

bool WeaselClient::Impl::GetResponseData(WeaselClient::ResponseHandler handler)
{
	using namespace boost::interprocess;
	try
	{
		shared_memory_object shm(
			open_only,
			SHARED_MEMORY_NAME,
			read_only
			);
		mapped_region region(
			shm, 
			read_only
			);

		void *addr = region.get_address();
		size_t size = region.get_size();
		
		return handler((LPWSTR)addr, size / sizeof(WCHAR));
	}
	catch (interprocess_exception& /*ex*/)
	{
		return false;
	}

	return false;
}