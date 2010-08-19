#include "stdafx.h"
#include "Deserializer.h"
#include "Action.h"

using namespace weasel;

static const wstring ACTION_NAME = L"action";

static struct ActionFactory
{
	ActionFactory()
	{
		Deserializer::Define(ACTION_NAME, ActionFactory::Create);
	}
	static Deserializer::Ptr Create(ResponseParser* pTarget)
	{
		return Deserializer::Ptr(new Action(pTarget));
	}

} action_factory;  // define this deserializer


Action::Action(ResponseParser* pTarget)
: Deserializer(pTarget)
{
}

Action::~Action()
{
}

void Action::Store(Deserializer::KeyType k, wstring const& value)
{
	Deserializer::KeyType end;
	if (k == end)  // "action" matches the entire key
	{
		// split value by L","
		vector<wstring> vecAction;
		split(vecAction, value, is_any_of(L","));
		// require specified action deserializers
		for(Deserializer::KeyType it = vecAction.begin(); it != vecAction.end(); ++it)
		{
			Deserializer::Require(*it, m_pTarget);
		}
	}
}
