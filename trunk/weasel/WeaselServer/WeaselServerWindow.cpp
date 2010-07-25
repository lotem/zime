#include "StdAfx.h"
#include "WeaselServerWindow.h"

WeaselServerWindow::WeaselServerWindow(void)
{
}

WeaselServerWindow::~WeaselServerWindow(void)
{
	//release all the client registery entry
	map<UINT, WeaselIPC*>::iterator it =	m_Clients.begin();
	for(; it != m_Clients.end(); it++)
	{
		delete (WeaselIPC*)it->second;
	}
}

BOOL WeaselServerWindow::PreTranslateMessage(MSG* pMsg)
{
	return PreTranslateMessage(pMsg);
}

BOOL WeaselServerWindow::OnIdle()
{
	return FALSE;
}

LRESULT WeaselServerWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);  

	return 0;
}

LRESULT WeaselServerWindow::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DestroyWindow();
	//quit the server
	::PostQuitMessage(0);
	return 0;
}

LRESULT WeaselServerWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	bHandled = FALSE;
	return 1;
}

void WeaselServerWindow::StartServer()
{
		
		HANDLE evt = OpenEvent( EVENT_ALL_ACCESS, FALSE,  SERVER_EVENT_NAME );
		::SetWindowText( m_hWnd,  SERVER_WND_NAME ); 
		if( evt != INVALID_HANDLE_VALUE )
		{
			SetEvent(evt);
			CloseHandle(evt);
		}
}

LRESULT WeaselServerWindow::OnClientEcho(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	map<UINT, WeaselIPC*>::iterator it =	m_Clients.find((UINT)lParam);
	if(it != m_Clients.end())//find
	{
		return   (LRESULT)lParam;
	}
	//client not registered.
	return (LRESULT)-1;
}

LRESULT WeaselServerWindow::OnAddClient(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	WeaselIPC* client = new WeaselIPC();
	m_Clients[(UINT)client] = client;
	return (LRESULT)client;
}

LRESULT WeaselServerWindow::OnRemoveClient(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	if ( m_Clients.erase((UINT)lParam)>0 )
	{   
		delete (WeaselIPC*) lParam;
	}
	return 0;
}

LRESULT WeaselServerWindow::OnSendKey(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	UINT keyEvent = (UINT)wParam;
	//keyEvent -> keyVal, mask
	UINT keyVal32 = (UINT) keyEvent && 0x0000ffff;
	keyEvent >>16;
	UINT mask = (UINT) keyEvent && 0x0000ffff;

	UINT clientID = (UINT)lParam;

	return 0;
}

LRESULT WeaselServerWindow::OnCloseServer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DestroyWindow();
	//quit the server
	::PostQuitMessage(0);
	return 0;
}