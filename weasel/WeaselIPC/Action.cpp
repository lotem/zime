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
	// "action" matches the entire key
	if (k == Deserializer::KeyType())
	{
		// TODO: split value by L","
		// foreach action in actions
		//     Deserializer::Require(action, m_pTarget);
	}
}
