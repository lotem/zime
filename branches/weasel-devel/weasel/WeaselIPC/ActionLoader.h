#pragma once
#include "Deserializer.h"

class ActionLoader : public weasel::Deserializer
{
public:
	ActionLoader(weasel::ResponseParser* pTarget);
	virtual ~ActionLoader();
	virtual void Store(weasel::Deserializer::KeyType k, std::wstring const& value);
};
