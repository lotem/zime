#pragma once
#include <WeaselIPC.h>
#include <map>	 
using std::map;

typedef CWinTraits<WS_DISABLED, WS_EX_TRANSPARENT> CWeaselServerTraits;

class WeaselServerWindow :
	public CWindowImpl<WeaselServerWindow, CWindow, CWeaselServerTraits>,
	public CMessageFilter, 
	public CIdleHandler
{
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	DECLARE_WND_CLASS (SERVER_WND_NAME)

	BEGIN_MSG_MAP(WeaselServerWindow)	   
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_WEASEL_CMD_ECHO, OnClientEcho)
		MESSAGE_HANDLER(WM_WEASEL_CMD_ADD_CLIENT, OnAddClient)			  
		MESSAGE_HANDLER(WM_WEASEL_CMD_REMOVE_CLIENT, OnRemoveClient)	
		MESSAGE_HANDLER(WM_WEASEL_CMD_SEND_KEY, OnSendKey)	
		MESSAGE_HANDLER(WM_WEASEL_CMD_CLOSE_SERVER, OnCloseServer)	
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnClientEcho(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnAddClient(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnRemoveClient(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSendKey(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseServer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
public:
	WeaselServerWindow(void);
	~WeaselServerWindow(void);
	void StartServer();

private:
	map<UINT, WeaselIPC*> m_Clients;
};
