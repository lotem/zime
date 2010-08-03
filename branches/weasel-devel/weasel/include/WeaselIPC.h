#pragma once
#include <WeaselCommon.h>

struct KeyEvent
{
	UINT keyCode : 16;
	UINT mask : 16;
	KeyEvent() : keyCode(0), mask(0) {}
	KeyEvent(UINT _keyCode, UINT _mask) : keyCode(_keyCode), mask(_mask) {}
	operator UINT() const
	{
		return *((UINT32*)this);
	}
};

// IPC接口

class WeaselClient
{
public:
	WeaselClient();
	virtual ~WeaselClient();
	
	// 连接到服务，必要时启动服务进程
	void ConnectServer();
	// 终止服务
	void ShutdownServer();
	// 请求服务处理按键消息
	bool ProcessKeyEvent(KeyEvent keyEvent);
	// 向server注册一个客户端。server返回一个ID用于标识客户端
	void AddClient();
	// 向server请求移除一个客户端(以ID标识)
	void RemoveClient();
	// ping server
	bool EchoFromServer();
	// 读取server返回的数据
	LPWSTR GetDataBuffer();
	UINT GetDataBufferLength();

private:
	class Impl;
	Impl* m_pImpl;
};

class WeaselServer
{
public:
	typedef LRESULT (*RequestHandler)(UINT cmd, WPARAM param, LPARAM clientID);

	WeaselServer();
	virtual ~WeaselServer();

	// 初始化服务
	int StartServer();
	// 结束服务
	int StopServer();
	// 消息循环
	int Run();
	// 注册回调函数
	void RegisterRequestHandler(RequestHandler handler);

private:
	class Impl;
	Impl* m_pImpl;
};
