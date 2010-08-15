#pragma once
#include <WeaselCommon.h>
#include <windows.h>
#include <boost/function.hpp>

#define WEASEL_IPC_SHARED_MEMORY "WeaselIPCSharedMemory"
#define WEASEL_IPC_BUFFER_SIZE 4096
#define WEASEL_IPC_BUFFER_LENGTH (WEASEL_IPC_BUFFER_SIZE / sizeof(WCHAR))

#define WEASEL_IPC_WINDOW L"WeaselIPCWindow"
#define WEASEL_IPC_READY_EVENT L"Local\\WeaselIPCReadyEvent"

enum WEASEL_IPC_COMMAND
{	
	WEASEL_IPC_ECHO = (WM_APP + 1),
	WEASEL_IPC_START_SESSION,
	WEASEL_IPC_END_SESSION,
	WEASEL_IPC_PROCESS_KEY_EVENT,
	WEASEL_IPC_SHUTDOWN_SERVER
};

namespace weasel
{

	struct KeyEvent
	{
		UINT keyCode : 16;
		UINT mask : 16;
		KeyEvent() : keyCode(0), mask(0) {}
		KeyEvent(UINT _keyCode, UINT _mask) : keyCode(_keyCode), mask(_mask) {}
		KeyEvent(UINT x)
		{
			*((UINT*)this) = x;
		}
		operator UINT() const
		{
			return *((UINT32*)this);
		}
	};

	struct RequestHandler
	{
		RequestHandler() {}
		virtual ~RequestHandler() {}
		virtual UINT FindSession(UINT sessionID) { return 0; }
		virtual UINT AddSession() { return 0; }
		virtual UINT RemoveSession(UINT sessionID) { return 0; }
		virtual BOOL ProcessKeyEvent(KeyEvent keyEvent, UINT sessionID, LPWSTR buffer) { return FALSE; }
	};
	typedef boost::function<bool (LPWSTR buffer, UINT length)> ResponseHandler;
	typedef boost::function<bool ()> ServerLauncher;

	// 實現類聲明

	class ClientImpl;
	class ServerImpl;

	// IPC接口類

	class Client
	{
	public:

		Client();
		virtual ~Client();
		
		// 连接到服务，必要时启动服务进程
		bool Connect(ServerLauncher launcher = 0);
		// 断开连接
		void Disconnect();
		// 终止服务
		void ShutdownServer();
		// 请求服务处理按键消息
		bool ProcessKeyEvent(KeyEvent keyEvent);
		// 發起會話
		void StartSession();
		// 結束會話
		void EndSession();
		// 测试连接
		bool Echo();
		// 读取server返回的数据
		bool GetResponseData(ResponseHandler handler);

	private:
		ClientImpl* m_pImpl;
	};

	class Server
	{
	public:
		Server(RequestHandler* pHandler = 0);
		virtual ~Server();

		// 初始化服务
		int Start();
		// 结束服务
		int Stop();
		// 消息循环
		int Run();

	private:
		ServerImpl* m_pImpl;
	};

}