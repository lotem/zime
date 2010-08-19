#include "stdafx.h"
#include "Deserializer.h"
#include "ActionLoader.h"

using namespace weasel;

static const wstring ACTION_NAME = L"action";

static struct ActionLoaderFactory
{
	ActionLoaderFactory()
	{
		Deserializer::Define(ACTION_NAME, ActionLoaderFactory::Create);
	}
	static Deserializer::Ptr Create(ResponseParser* pTarget)
	{
		return Deserializer::Ptr(new ActionLoader(pTarget));
	}

} action_loader_factory;  // define this deserializer


ActionLoader::ActionLoader(ResponseParser* pTarget)
: Deserializer(pTarget)
{
}

ActionLoader::~ActionLoader()
{
}

void ActionLoader::Store(Deserializer::KeyType k, wstring const& value)
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
