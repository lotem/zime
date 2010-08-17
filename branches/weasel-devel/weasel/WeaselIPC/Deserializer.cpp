#include "stdafx.h"
#include "Deserializer.h"

using namespace weasel;

Deserializer::Deserializer(ResponseParser* pTarget)
: m_pTarget(pTarget)
{
}

Deserializer::~Deserializer()
{
}

void Deserializer::Feed(const wstring& line)
{
	// ignore blank lines and comments
	if (line.empty() || line.find_first_of(L'#') == 0)
		return;

	vector<wstring> key;
	wstring value;

	// TODO: extract key (split by L'.') and value
	// cf. boost::string_algo
	if (key.empty())
		return;

	_Dispatch(key.begin(), value);
}

void Deserializer::_Dispatch(Deserializer::KeyType k, wstring const& value)
{
	// first part of the key as action type
	wstring const& action = *k;
	
	// get required action deserializer instance
	map<wstring, Ptr>::iterator i = s_instances.find(action);
	if (i == s_instances.end())
	{
		// line ignored... since corresponding deserializer is not active
		return;
	}

	// restore data structure
	Ptr p = i->second;
	p->Store(++k, value);
}

void Deserializer::Define(std::wstring const& action, Factory factory)
{
	s_factories[action] = factory;
}

bool Deserializer::Require(std::wstring const& action, ResponseParser* pTarget)
{
	map<wstring, Factory>::iterator i = s_factories.find(action);
	if (i == s_factories.end())
	{
		// unknown action type
		return false;
	}
	s_instances[action] = i->second(pTarget);
	return true;
}