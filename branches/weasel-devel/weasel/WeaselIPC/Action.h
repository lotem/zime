#pragma once
#include "ICollectable.h"

typedef 
enum _ACTIONTYPE
{
	NOOP,
	COMMIT,
	UPDATE_CTX,
	UPDATE_STATUS
}ACTIONTYPE;

namespace weasel
{
	class CAction : ICollectable
	{
	public:
		CAction(void)
			:actionType(NOOP)
		{
		}

		virtual ~CAction(void)
		{
		}

		virtual bool Collect(const string& str)
		{
			return true;
		}

	private:
		ACTIONTYPE actionType;
	};
}
