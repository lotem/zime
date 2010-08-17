#include "stdafx.h"
#include <WeaselIPC.h>
#include "Deserializer.h"

using namespace weasel;

ResponseParser::ResponseParser(std::wstring& commit, Context& context, Status& status)
 : r_commit(commit), r_context(context), r_status(status)
{
	Deserializer::Require(L"action", this);
}

bool ResponseParser::operator() (LPWSTR buffer, UINT length)
{
	wbufferstream bs(buffer, length);
	wstring line;
	while (bs.good())
	{
		getline(bs, line);
		if (!bs.good())
			return false;
		
		Feed(line);
	}
	return bs.good();
}

void ResponseParser::Feed(const wstring& line)
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

	Deserializer::KeyType k = key.begin();

	// first part of the key serve as action type
	wstring const& action = *k;
	
	// get required action deserializer instance
	map<wstring, Deserializer::Ptr>::iterator i = deserializers.find(action);
	if (i == deserializers.end())
	{
		// line ignored... since corresponding deserializer is not active
		return;
	}

	// dispatch
	Deserializer::Ptr p = i->second;
	p->Store(++k, value);
}
