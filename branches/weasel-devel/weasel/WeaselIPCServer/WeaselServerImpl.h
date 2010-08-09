#pragma once
#include <WeaselIPC.h>

// WeaselServer::Impl definition

typedef CWinTraits<WS_DISABLED, WS_EX_TRANSPARENT> WeaselServerWinTraits;

class WeaselServer::Impl :
	public CWindowImpl<WeaselServer::Impl, CWindow, WeaselServerWinTraits>
{
public:
	DECLARE_WND_CLASS (WEASEL_IPC_WINDOW)

	BEGIN_MSG_MAP(WeaselServerWindow)	   
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WEASEL_IPC_ECHO, OnClientEcho)
		MESSAGE_HANDLER(WEASEL_IPC_ADD_CLIENT, OnAddClient)			  
		MESSAGE_HANDLER(WEASEL_IPC_REMOVE_CLIENT, OnRemoveClient)	
		MESSAGE_HANDLER(WEASEL_IPC_PROCESS_KEY_EVENT, OnKeyEvent)	
		MESSAGE_HANDLER(WEASEL_IPC_SHUTDOWN_SERVER, OnShutdownServer)	
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClientEcho(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnAddClient(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRemoveClient(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShutdownServer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	Impl(RequestHandler* pHandler);
	~Impl();

	int StartServer();
	int StopServer();
	int Run();
	void RegisterRequestHandler(WeaselServer::RequestHandler handler);

private:
	RequestHandler* m_pHandler;
	class SharedMemory;
	SharedMemory* m_pSharedMemory;
};
