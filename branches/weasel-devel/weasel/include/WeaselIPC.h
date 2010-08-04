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

// IPC�ӿ�

class WeaselClient
{
public:
	typedef boost::function<bool (LPWSTR buffer, UINT length)> ResponseHandler;
	typedef boost::function<bool ()> ServerLauncher;

	WeaselClient();
	virtual ~WeaselClient();
	
	// ���ӵ����񣬱�Ҫʱ�����������
	bool ConnectServer(ServerLauncher launcher = 0);
	// ��ֹ����
	void ShutdownServer();
	// �������������Ϣ
	bool ProcessKeyEvent(KeyEvent keyEvent);
	// ��serverע��һ���ͻ��ˡ�server����һ��ID���ڱ�ʶ�ͻ���
	void AddClient();
	// ��server�����Ƴ�һ���ͻ���(��ID��ʶ)
	void RemoveClient();
	// ping server
	bool EchoFromServer();
	// ��ȡserver���ص�����
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

	// ��ʼ������
	int StartServer();
	// ��������
	int StopServer();
	// ��Ϣѭ��
	int Run();

private:
	class Impl;
	Impl* m_pImpl;
};
