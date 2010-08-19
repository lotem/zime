#pragma once
#include "Deserializer.h"

class Contexter : public weasel::Deserializer
{
public:
	Contexter(weasel::ResponseParser* pTarget);
	virtual ~Contexter();
	virtual void Store(weasel::Deserializer::KeyType const& key, std::wstring const& value);
	
	void _StorePreedit(Deserializer::KeyType k, wstring const& value);
	void _StoreAux(Deserializer::KeyType k, wstring const& value);
	void _StoreCand(Deserializer::KeyType k, wstring const& value);

	static weasel::Deserializer::Ptr Create(weasel::ResponseParser* pTarget);
};