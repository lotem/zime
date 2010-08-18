#include "stdafx.h"
#include "Deserializer.h"
#include "Commit.h"

using namespace weasel;


static const wstring COMMIT_NAME = L"commit";

static struct CommitFactory
{
	CommitFactory()
	{
		Deserializer::Define(COMMIT_NAME, CommitFactory::Create);
	}
	static Deserializer::Ptr Create(ResponseParser* pTarget)
	{
		return Deserializer::Ptr(new Commit(pTarget));
	}

} Commit_factory;  // define this deserializer


Commit::Commit(ResponseParser* pTarget)
: Deserializer(pTarget)
{
}

Commit::~Commit()
{
}

void Commit::Store(Deserializer::KeyType k, wstring const& value)
{
	//Store Commit str
	if (k == Deserializer::KeyType())
		m_pTarget->r_commit = value;
}
