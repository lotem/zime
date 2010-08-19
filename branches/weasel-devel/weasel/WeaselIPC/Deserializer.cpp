#include "stdafx.h"
#include "Deserializer.h"

using namespace weasel;


map<wstring, Deserializer::Factory> Deserializer::s_factories;


void Deserializer::Define(std::wstring const& action, Factory factory)
{
	s_factories[action] = factory;
}

bool Deserializer::Require(std::wstring const& action, ResponseParser* pTarget)
{
	if (!pTarget)
		return false;

	map<wstring, Factory>::iterator i = s_factories.find(action);
	if (i == s_factories.end())
	{
		// unknown action type
		return false;
	}

	Factory& factory = i->second;
	pTarget->deserializers[action] = factory(pTarget);
	return true;
}