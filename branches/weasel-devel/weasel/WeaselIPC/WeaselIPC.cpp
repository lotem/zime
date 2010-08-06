#pragma once
#include "stdafx.h"
#include <WeaselIPC.h>
#include "WeaselIPCImpl.h"

// WeaselClient

WeaselClient::WeaselClient() 
	: m_pImpl(new WeaselClient::Impl())
{}

WeaselClient::~WeaselClient()
{
	if (m_pImpl)
		delete m_pImpl;
}

bool WeaselClient::ConnectServer(ServerLauncher launcher)
{
	return m_pImpl->ConnectServer(launcher);
}

void WeaselClient::DisconnectServer()
{
	m_pImpl->DisconnectServer();
}

void WeaselClient::ShutdownServer()
{
	m_pImpl->ShutdownServer();
}

bool WeaselClient::ProcessKeyEvent(KeyEvent keyEvent)
{
	return m_pImpl->ProcessKeyEvent(keyEvent);
}

void WeaselClient::AddClient()
{
	m_pImpl->AddClient();
}

void WeaselClient::RemoveClient()
{
	m_pImpl->RemoveClient();
}

bool WeaselClient::EchoFromServer()
{
	return m_pImpl->EchoFromServer();
}

bool WeaselClient::GetResponseData(WeaselClient::ResponseHandler handler)
{
	return m_pImpl->GetResponseData(handler);
}

// WeaselServer

WeaselServer::WeaselServer(WeaselServer::RequestHandler* pHandler)
	: m_pImpl(new WeaselServer::Impl(pHandler))
{}

WeaselServer::~WeaselServer()
{
	if (m_pImpl)
		delete m_pImpl;
}

int WeaselServer::StartServer()
{
	return m_pImpl->StartServer();
}

int WeaselServer::StopServer()
{
	return m_pImpl->StopServer();
}

int WeaselServer::Run()
{
	return m_pImpl->Run();
}
