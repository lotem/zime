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

void WeaselClient::ConnectServer()
{
	m_pImpl->ConnectServer();
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

LPWSTR WeaselClient::GetDataBuffer()
{
	return m_pImpl->GetDataBuffer();
}

UINT WeaselClient::GetDataBufferLength()
{
	return DATA_BUFFER_SIZE / sizeof(WCHAR);
}

// WeaselServer

WeaselServer::WeaselServer()
	: m_pImpl(new WeaselServer::Impl())
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

void WeaselServer::RegisterRequestHandler(WeaselServer::RequestHandler handler)
{
	m_pImpl->RegisterRequestHandler(handler);
}
