#pragma once
#include "stdafx.h"
#include "WeaselIPC.h"
#include "WeaselIPCImpl.h"

WeaselIPC::WeaselIPC(void) 
	: m_pImpl(new WeaselIPCImpl())
{}

WeaselIPC::~WeaselIPC(void)
{
	delete	 m_pImpl;
}

//IPC Functions
void WeaselIPC::ConnectServer()
{
	m_pImpl->ConnectServer();
}

void WeaselIPC::CloseServer()
{
	m_pImpl->CloseServer();
}

void WeaselIPC::SendKey(UINT keyVal, LONG mask)
{
	m_pImpl->SendKey(keyVal, mask);
}

void WeaselIPC::AddClient()
{
	m_pImpl->AddClient();
}

void WeaselIPC::RemoveClient()
{
	m_pImpl->RemoveClient();
}

UINT WeaselIPC::EchoFromServer()
{
	return m_pImpl->EchoFromServer();
}

UCHAR* WeaselIPC::GetDataFromSharedMem()
 {
	return NULL;
}