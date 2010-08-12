#pragma once
#include <WeaselIPC.h>
#include "Action.h"
#include "Context.h"
#include "Commit.h"
#include "Status.h"

using namespace weasel;

namespace weasel
{
	struct ResponseImpl
	{
		ResponseImpl();
		~ResponseImpl();
		bool Parse(const std::string& str);
		CAction m_action;
		CContext m_ctx;
		CCommit m_commit;
		CStatus m_status;
	};

}