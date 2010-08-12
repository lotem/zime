#pragma once
#include "ICollectable.h"
#include "WeaselCommon.h"

namespace weasel
{
	class CStatus : ICollectable
	{
	public:
		CStatus(void)
		{
		}

		~CStatus(void)
		{
		}

		virtual bool Collect(const string& str)
		{
			return true;
		}

	private:
		weasel::Status m_status;
	};
}
