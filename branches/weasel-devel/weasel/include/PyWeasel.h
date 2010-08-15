#pragma once
#include <WeaselIPC.h>
#include <boost/python.hpp>

class PyWeaselHandler :
	public weasel::RequestHandler
{
public:
	PyWeaselHandler();
	virtual ~PyWeaselHandler();
	virtual UINT FindSession(UINT sessionID);
	virtual UINT AddSession();
	virtual UINT RemoveSession(UINT sessionID);
	virtual BOOL ProcessKeyEvent(weasel::KeyEvent keyEvent, UINT sessionID, LPWSTR buffer);

private:
	boost::python::object m_service;
};
