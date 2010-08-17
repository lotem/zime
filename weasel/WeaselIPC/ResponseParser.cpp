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
		
		Deserializer::Feed(line);
	}
	return bs.good();
}
