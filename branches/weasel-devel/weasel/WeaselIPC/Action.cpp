#include "stdafx.h"
#include "Deserializer.h"
#include "Action.h"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string.hpp"

using namespace std;
using namespace weasel;
using namespace boost::algorithm;

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
	// "action" matches the entire key
	if (k == Deserializer::KeyType())
	{
		// TODO: split value by L","
		typedef vector<std::wstring > split_vector_type;
		split_vector_type vecAction;
		split( vecAction, value, is_any_of(","));
		
		for(split_vector_type::const_iterator it = vecAction.begin(); it != vecAction.end(); ++it)
		{
			Deserializer::Require(*it, m_pTarget);
		}
	}
}
