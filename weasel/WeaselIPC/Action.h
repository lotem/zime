#pragma once
#include "Deserializer.h"

class Action : public weasel::Deserializer
{
public:
	Action(weasel::ResponseParser* pTarget);
	virtual ~Action();
	virtual void Store(weasel::Deserializer::KeyType k, std::wstring const& value);
};
