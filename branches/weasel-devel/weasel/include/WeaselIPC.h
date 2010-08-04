#pragma once
#include <WeaselCommon.h>
#include <windows.h>
#include <boost/function.hpp>

struct KeyEvent
{
	UINT keyCode : 16;
	UINT mask : 16;
	KeyEvent() : keyCode(0), mask(0) {}
	KeyEvent(UINT x)
	{
		*((UINT*)this) = x;
	}
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
	typedef boost::function<bool (LPWSTR buffer, UINT length)> ResponseHandler;
	typedef boost::function<bool ()> ServerLauncher;

	WeaselClient();
	virtual ~WeaselClient();
	
	// 连接到服务，必要时启动服务进程
	bool ConnectServer(ServerLauncher launcher = 0);
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
	bool GetResponseData(ResponseHandler handler);

private:
	class Impl;
	Impl* m_pImpl;
};

class WeaselServer
{
public:
	struct RequestHandler
	{
		RequestHandler() {}
		virtual ~RequestHandler() {}
		virtual UINT FindClient(UINT clientID) { return 0; }
		virtual UINT AddClient() { return 0; }
		virtual UINT RemoveClient(UINT clientID) { return 0; }
		virtual BOOL ProcessKeyEvent(KeyEvent keyEvent, UINT clientID) { return FALSE; }
	};

	WeaselServer(RequestHandler* pHandler = 0);
	virtual ~WeaselServer();

	// 初始化服务
	int StartServer();
	// 结束服务
	int StopServer();
	// 消息循环
	int Run();

private:
	class Impl;
	Impl* m_pImpl;
};
