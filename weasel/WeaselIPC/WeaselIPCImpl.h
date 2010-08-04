#pragma once
#include <WeaselIPC.h>
#include <windows.h>

enum WEASEL_CMD
{	
	WM_WEASEL_CMD_ECHO = (WM_APP + 1),
	WM_WEASEL_CMD_ADD_CLIENT,
	WM_WEASEL_CMD_REMOVE_CLIENT,
	WM_WEASEL_CMD_PROCESS_KEY_EVENT,
	WM_WEASEL_CMD_SHUTDOWN_SERVER
};

#define SHARED_MEMORY_NAME "WeaselIPCSharedMemory"
#define DATA_BUFFER_SIZE 4096

#define SERVER_WND_NAME L"WeaselServerWindow"
#define SERVER_EVENT_NAME L"Local\\WeaselServerEvent"

#define SERVER_EXEC L"WeaselServer.exe"
#define SERVER_ARGS NULL
#define SERVER_DIR NULL

// WeaselClient::Impl definition

class WeaselClient::Impl
{
public:
	Impl();
	~Impl();

	void ConnectServer();
	void ShutdownServer();
	bool ProcessKeyEvent(KeyEvent keyEvent);
	void AddClient();
	void RemoveClient();
	bool EchoFromServer();
	bool GetResponseData(ResponseHandler handler);

private:
	UINT clientID;
	HWND serverWnd;
};

// WeaselServer::Impl definition

typedef CWinTraits<WS_DISABLED, WS_EX_TRANSPARENT> WeaselServerWinTraits;

class WeaselServer::Impl :
	public CWindowImpl<WeaselServer::Impl, CWindow, WeaselServerWinTraits>
{
public:
	DECLARE_WND_CLASS (SERVER_WND_NAME)

	BEGIN_MSG_MAP(WeaselServerWindow)	   
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_WEASEL_CMD_ECHO, OnClientEcho)
		MESSAGE_HANDLER(WM_WEASEL_CMD_ADD_CLIENT, OnAddClient)			  
		MESSAGE_HANDLER(WM_WEASEL_CMD_REMOVE_CLIENT, OnRemoveClient)	
		MESSAGE_HANDLER(WM_WEASEL_CMD_PROCESS_KEY_EVENT, OnKeyEvent)	
		MESSAGE_HANDLER(WM_WEASEL_CMD_SHUTDOWN_SERVER, OnShutdownServer)	
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
	Impl();
	~Impl();

	int StartServer();
	int StopServer();
	int Run();
	void RegisterRequestHandler(WeaselServer::RequestHandler handler);

private:
	RequestHandler m_handler;
	class SharedMemory;
	SharedMemory* m_pSharedMemory;
};
