#include "StdAfx.h"
#include "WeaselIPCImpl.h"

WeaselIPC::WeaselIPCImpl::WeaselIPCImpl()
	:clientID(0),
	 serverWnd(NULL),
	 sharedMem(INVALID_HANDLE_VALUE)
{
}

WeaselIPC::WeaselIPCImpl::~WeaselIPCImpl()
{
}

void WeaselIPC::WeaselIPCImpl::ConnectServer()
{
}

void WeaselIPC::WeaselIPCImpl::SendKey(UINT keyVal, LONG mask)
{

}

UINT WeaselIPC::WeaselIPCImpl::AddClient()
{
	return 0;
}

void WeaselIPC::WeaselIPCImpl::RemoveClient(UINT clientID)
{

}

UINT WeaselIPC::WeaselIPCImpl::EchoFromServer()
{
	return 0;
}