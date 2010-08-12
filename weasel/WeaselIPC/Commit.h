#pragma once
#include "ICollectable.h"

namespace weasel
{
	class CCommit : ICollectable
	{
	public:
		CCommit(void)
		{
		}
		~CCommit(void)
		{
		}

		virtual bool Collect(const string& str)
		{
			return true;
		}
	private:
		string m_CommitStr;
	};
}
