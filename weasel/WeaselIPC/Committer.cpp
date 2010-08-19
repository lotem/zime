#include "stdafx.h"
#include "Deserializer.h"
#include "Committer.h"

using namespace weasel;


static const wstring ACTION_NAME = L"commit";

static struct CommitterFactory
{
	CommitterFactory()
	{
		Deserializer::Define(ACTION_NAME, CommitterFactory::Create);
	}
	static Deserializer::Ptr Create(ResponseParser* pTarget)
	{
		return Deserializer::Ptr(new Committer(pTarget));
	}

} committer_factory;  // define this deserializer


Committer::Committer(ResponseParser* pTarget)
: Deserializer(pTarget)
{
}

Committer::~Committer()
{
}

void Committer::Store(Deserializer::KeyType k, wstring const& value)
{
	//Store Commit str
	if (k == Deserializer::KeyType())
		m_pTarget->r_commit = value;
}
