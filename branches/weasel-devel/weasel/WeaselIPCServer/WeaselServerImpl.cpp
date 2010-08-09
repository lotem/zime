#include "stdafx.h"
#include "WeaselServerImpl.h"

extern CAppModule _Module;

class WeaselServer::Impl::SharedMemory
{
public:
	SharedMemory() 
	{
		m_pShm = new windows_shared_memory(create_only, 
			                               WEASEL_IPC_SHARED_MEMORY, 
										   read_write, 
										   WEASEL_IPC_BUFFER_SIZE);
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

WeaselServer::Impl::Impl(WeaselServer::RequestHandler* pHandler)
: m_pHandler(pHandler), m_pSharedMemory(new SharedMemory())
{
}

WeaselServer::Impl::~Impl()
{
	if (m_pHandler)
		delete m_pHandler;
	if (m_pSharedMemory)
		delete m_pSharedMemory;
}

LRESULT WeaselServer::Impl::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// clients connects to server via calls to FindWindow() with SERVER_WND_NAME
	::SetWindowText( m_hWnd,  WEASEL_IPC_WINDOW ); 

	// notify clients that we are ready serving
	HANDLE hEvent = OpenEvent( EVENT_ALL_ACCESS, FALSE,  WEASEL_IPC_READY_EVENT );
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
	bHandled = FALSE;
	return 1;
}

int WeaselServer::Impl::StartServer()
{
	// assure single instance
	if (FindWindow(WEASEL_IPC_WINDOW, NULL) != NULL)
	{
		return 0;
	}

	HWND hwnd = Create(NULL);
	return (int)hwnd;
}

int WeaselServer::Impl::StopServer()
{
	if (!IsWindow())
	{
		return 0;
	}
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

LRESULT WeaselServer::Impl::OnClientEcho(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_pHandler)
		return 0;
	return m_pHandler->FindClient(lParam);
}

LRESULT WeaselServer::Impl::OnAddClient(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_pHandler)
		return 0;
	return m_pHandler->AddClient();
}

LRESULT WeaselServer::Impl::OnRemoveClient(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_pHandler)
		return 0;
	return m_pHandler->RemoveClient(lParam);
}

LRESULT WeaselServer::Impl::OnKeyEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_pHandler || !m_pSharedMemory)
		return 0;
	return m_pHandler->ProcessKeyEvent(KeyEvent(wParam), lParam, m_pSharedMemory->GetBuffer());
}

LRESULT WeaselServer::Impl::OnShutdownServer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	StopServer();
	return 0;
}

// WeaselServer

WeaselServer::WeaselServer(WeaselServer::RequestHandler* pHandler)
	: m_pImpl(new WeaselServer::Impl(pHandler))
{}

WeaselServer::~WeaselServer()
{
	if (m_pImpl)
		delete m_pImpl;
}

int WeaselServer::StartServer()
{
	return m_pImpl->StartServer();
}

int WeaselServer::StopServer()
{
	return m_pImpl->StopServer();
}

int WeaselServer::Run()
{
	return m_pImpl->Run();
}
