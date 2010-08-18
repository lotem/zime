#pragma once
#include "Deserializer.h"

class Commit : public weasel::Deserializer
{
public:
	Commit(weasel::ResponseParser* pTarget);
	virtual ~Commit();
	virtual void Store(weasel::Deserializer::KeyType k, std::wstring const& value);
};
