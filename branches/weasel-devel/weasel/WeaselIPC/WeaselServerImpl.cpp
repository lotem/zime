#include "stdafx.h"
#include "WeaselIPCImpl.h"

class WeaselServer::Impl::SharedMemory
{
public:
	SharedMemory() 
	{
		m_pShm = new windows_shared_memory(create_only, SHARED_MEMORY_NAME, read_write, DATA_BUFFER_SIZE);
		m_pRegion = new mapped_region(*m_pShm, read_write);
	}
	~SharedMemory()
	{
		delete m_pRegion;
		delete m_pShm;
	}

	LPWSTR GetBuffer()
	{
		return (LPWSTR)m_pRegion->get_address();
	}

private:
	windows_shared_memory* m_pShm;
	mapped_region* m_pRegion;
};

WeaselServer::Impl::Impl()
: m_handler(0), m_pSharedMemory(0)
{
}

WeaselServer::Impl::~Impl()
{
	if (m_pSharedMemory)
		delete m_pSharedMemory;
}

LRESULT WeaselServer::Impl::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_pSharedMemory = new SharedMemory();

	// clients connects to server via calls to FindWindow() with SERVER_WND_NAME
	::SetWindowText( m_hWnd,  SERVER_WND_NAME ); 

	// notify clients that we are ready serving
	HANDLE hEvent = OpenEvent( EVENT_ALL_ACCESS, FALSE,  SERVER_EVENT_NAME );
	if( hEvent != INVALID_HANDLE_VALUE )
	{
		SetEvent(hEvent);
		CloseHandle(hEvent);
	}

	return 0;
}

LRESULT WeaselServer::Impl::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	StopServer();
	return 0;
}

LRESULT WeaselServer::Impl::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// TODO: clean up engines

	bHandled = FALSE;
	return 1;
}

int WeaselServer::Impl::StartServer()
{
	// assure single instance
	if (FindWindow(SERVER_WND_NAME, NULL) != NULL)
	{
		return 0;
	}

	HWND hwnd = Create(NULL);
	return (int)hwnd;
}

int WeaselServer::Impl::StopServer()
{
	DestroyWindow();
	//quit the server
	::PostQuitMessage(0);
	return 0;
}

int WeaselServer::Impl::Run()
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);
	
	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

void WeaselServer::Impl::RegisterRequestHandler(WeaselServer::RequestHandler handler)
{
	m_handler = handler;
}

LRESULT WeaselServer::Impl::OnClientEcho(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	// TODO: lookup clientID
	// client not registered
	return 0;
}

LRESULT WeaselServer::Impl::OnAddClient(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: 
	return 0;
}

LRESULT WeaselServer::Impl::OnRemoveClient(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	// TODO: 
	return 0;
}

LRESULT WeaselServer::Impl::OnKeyEvent(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	// TODO:
	return 0;
}

LRESULT WeaselServer::Impl::OnShutdownServer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	StopServer();
	return 0;
}