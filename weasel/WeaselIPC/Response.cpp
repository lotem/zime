#pragma once
#include "stdafx.h"
#include "WeaselIPC.h"
#include "Action.h"
#include "Context.h"
#include "Commit.h"
#include "Status.h"
#include "ResponseImpl.h"

weasel::CResponse::CResponse()
	:m_Impl(new ResponseImpl())
{
}

weasel::CResponse::~CResponse()
{
	delete m_Impl;
}

bool weasel::CResponse::Parse(const string& str)
{
	return m_Impl->Parse(str);
}

CAction weasel::CResponse::GetAction()
{
	return m_Impl->m_action;
}
CCommit weasel::CResponse::GetCommit()
{
	return m_Impl->m_commit;
}
CContext weasel::CResponse::GetContext()
{
	return m_Impl->m_ctx;
}
CStatus weasel::CResponse::GetStatus()
{
	return m_Impl->m_status;
}