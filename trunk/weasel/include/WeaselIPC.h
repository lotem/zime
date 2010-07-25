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

//IPC�ӿ�
class WeaselIPC
{
public:
	WeaselIPC(void) ;
	~WeaselIPC(void);
	
	//��ʼ��IPC server
	void ConnectServer();
	 //�ر�IPC server
	void CloseServer();
	//��server����һ����
	void SendKey(UINT keyVal, LONG mask);
	//��serverע��һ���ͻ��ˡ�server����һ��ID���ڱ�ʶ�ͻ���
	void AddClient();
	//��server�����Ƴ�һ���ͻ���(��ID��ʶ)
	void RemoveClient();
	//ping server �ɹ����� 1, ʧ�ܷ��� 0
	UINT EchoFromServer();

	//�ӹ����ڴ�����ȡserver���صĶ���
	UCHAR* GetDataFromSharedMem();

private:
	class WeaselIPCImpl;
	WeaselIPCImpl* m_pImpl;
};
