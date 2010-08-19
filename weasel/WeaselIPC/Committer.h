#pragma once
#include "Deserializer.h"

class Committer : public weasel::Deserializer
{
public:
	Committer(weasel::ResponseParser* pTarget);
	virtual ~Committer();
	virtual void Store(weasel::Deserializer::KeyType k, std::wstring const& value);
};
