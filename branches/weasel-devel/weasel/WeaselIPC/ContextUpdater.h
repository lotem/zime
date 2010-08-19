#pragma once
#include "Deserializer.h"

class ContextUpdater : public weasel::Deserializer
{
public:
	ContextUpdater(weasel::ResponseParser* pTarget);
	virtual ~ContextUpdater();
	virtual void Store(weasel::Deserializer::KeyType const& key, std::wstring const& value);
	
	void _StorePreedit(Deserializer::KeyType k, wstring const& value);
	void _StoreAux(Deserializer::KeyType k, wstring const& value);
	void _StoreCand(Deserializer::KeyType k, wstring const& value);

	static weasel::Deserializer::Ptr Create(weasel::ResponseParser* pTarget);
};