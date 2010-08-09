#pragma once
#include <WeaselIPC.h>

class WeaselClient::Impl
{
public:
	Impl();
	~Impl();

	bool ConnectServer(ServerLauncher const& launcher);
	void DisconnectServer();
	void ShutdownServer();
	bool ProcessKeyEvent(KeyEvent keyEvent);
	void AddClient();
	void RemoveClient();
	bool EchoFromServer();
	bool GetResponseData(ResponseHandler const& handler);

protected:
	bool _Connected() const { return serverWnd != NULL; }
	bool _Active() const { return _Connected() && clientID != 0; }

private:
	UINT clientID;
	HWND serverWnd;
};
