#include "stdafx.h"
#include <PyWeasel.h>

PyWeaselHandler::PyWeaselHandler()
{
	try
	{
		python::object module = python::import("engine.weasel");
		m_service = module.attr("service");
	}
	catch (python::error_already_set e)
	{
		python::handle_exception();
	}

	if (m_service.is_none())
	{
		throw std::exception("weasel service not found!");
	}
}

PyWeaselHandler::~PyWeaselHandler()
{
	try
	{
		m_service.attr("cleanup")();
	}
	catch (python::error_already_set e)
	{
	}
}

UINT PyWeaselHandler::FindSession(UINT sessionID)
{
	bool found = false;
	try
	{
		found = python::extract<bool>(m_service.attr("has_session")(sessionID));
	}
	catch (python::error_already_set e)
	{
		return 0;
	}
	return found ? sessionID : 0;
}

UINT PyWeaselHandler::AddSession()
{
	UINT id = 0;
	try
	{
		id = python::extract<UINT>(m_service.attr("create_session")());
	}
	catch (python::error_already_set e)
	{
		return 0;
	}
	return id;
}

UINT PyWeaselHandler::RemoveSession(UINT sessionID)
{
	bool done = false;
	try
	{
		done = python::extract<bool>(m_service.attr("destroy_session")(sessionID));
	}
	catch (python::error_already_set e)
	{
		return 0;
	}
	return done ? sessionID : 0;
}

BOOL PyWeaselHandler::ProcessKeyEvent(weasel::KeyEvent keyEvent, UINT sessionID, LPWSTR buffer)
{
	wstring response;
	try
	{
		python::object session = m_service.attr("get_session")(sessionID);
		if (session.is_none())
		{
			return FALSE;
		}
		
		python::object ret = session.attr("process_key_event")(keyEvent.keyCode, keyEvent.mask);
		if (ret.is_none())
		{
			return FALSE;
		}

		response = python::extract<wstring>(ret);
	}
	catch (python::error_already_set e)
	{
		return FALSE;
	}

	wbufferstream bs(buffer, WEASEL_IPC_BUFFER_SIZE / sizeof(WCHAR));
	bs << response;
	if (!bs.good())
	{
		return FALSE;
	}

	return TRUE;
}
