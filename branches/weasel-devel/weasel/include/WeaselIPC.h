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

// IPC�ӿ�

class WeaselClient
{
public:
	WeaselClient();
	virtual ~WeaselClient();
	
	// ���ӵ����񣬱�Ҫʱ�����������
	void ConnectServer();
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

	// ��ʼ������
	int StartServer();
	// ��������
	int StopServer();
	// ��Ϣѭ��
	int Run();
	// ע��ص�����
	void RegisterRequestHandler(RequestHandler handler);

private:
	class Impl;
	Impl* m_pImpl;
};
