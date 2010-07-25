#pragma once
#include <WeaselCommon.h>
#include <Windows.h>

#define SERVER_WND_NAME L"WeaselServerWindow"
#define SERVER_EVENT_NAME L"Local\\WeaselServer"

enum WEASEL_CMD
{	
	WM_WEASEL_CMD_ECHO = (WM_APP + 1),
	WM_WEASEL_CMD_ADD_CLIENT,
	WM_WEASEL_CMD_REMOVE_CLIENT,
	WM_WEASEL_CMD_SEND_KEY,
	WM_WEASEL_CMD_CLOSE_SERVER
};

//IPC接口
class WeaselIPC
{
public:
	WeaselIPC(void) ;
	~WeaselIPC(void);
	
	//初始化IPC server
	void ConnectServer();
	 //关闭IPC server
	void CloseServer();
	//向server发送一个键
	void SendKey(UINT keyVal, LONG mask);
	//向server注册一个客户端。server返回一个ID用于标识客户端
	void AddClient();
	//向server请求移除一个客户端(以ID标识)
	void RemoveClient();
	//ping server 成功返回 1, 失败返回 0
	UINT EchoFromServer();

	//从共享内存区读取server返回的对象
	UCHAR* GetDataFromSharedMem();

private:
	class WeaselIPCImpl;
	WeaselIPCImpl* m_pImpl;
};
