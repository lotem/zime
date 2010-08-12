#pragma once

#include "ICollectable.h"
#include "WeaselCommon.h"

namespace weasel
{
	class CContext : ICollectable
	{
	public:
		CContext(void)
		{
		}

		virtual ~CContext(void)
		{
		}

		virtual bool Collect(const string& str)
		{
			return true;
		}

	private:
		weasel::Context m_ctx;
	};
}
