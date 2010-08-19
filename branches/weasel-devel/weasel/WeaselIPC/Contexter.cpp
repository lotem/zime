#include "stdafx.h"
#include "Deserializer.h"
#include "Contexter.h"

using namespace weasel;

Deserializer::Ptr Contexter::Create(ResponseParser* pTarget)
{
	return Deserializer::Ptr(new Contexter(pTarget));
}


Contexter::Contexter(ResponseParser* pTarget)
: Deserializer(pTarget)
{
}

Contexter::~Contexter()
{
}

void Contexter::Store(Deserializer::KeyType const& k, wstring const& value)
{
	if(k.size() <2)
		return;

	if (k[1] == L"preedit")
	{
		_StorePreedit(k, value);
		return;
	}

	if (k[1] == L"aux")
	{
		_StoreAux(k, value);
		return;
	}
	if (k[1] == L"cand")
	{
		_StoreCand(k, value);
		return;
	}
	if (k[1] == L"pages")
	{
		vector<wstring> vec;
		split(vec, value, is_any_of(L"/"));

		int i = _wtoi(vec[0].c_str());
		int n = _wtoi(vec[1].c_str());
		m_pTarget->r_context.cinfo.currentPage = i;
		m_pTarget->r_context.cinfo.totalPages = n;
		return;
	}
}

void Contexter::_StorePreedit(Deserializer::KeyType k, wstring const& value)
{
	if(k.size() == 2)
	{
		m_pTarget->r_context.preedit.str = value;
		return;
	}
	if(k.size() == 3)
	{
		if (k[2] == L"cursor")
		{
			vector<wstring> vec;
			split(vec, value, is_any_of(L","));

			weasel::TextAttribute attr;
			attr.type = HIGHLIGHTED;
			attr.range.start = _wtoi(vec[0].c_str());
			attr.range.end = _wtoi(vec[1].c_str());
			
			m_pTarget->r_context.preedit.attributes.push_back(attr);
			return;
		}
	}
}

void Contexter::_StoreAux(Deserializer::KeyType k, wstring const& value)
{
	if(k.size() == 2)
	{
		m_pTarget->r_context.aux.str = value;
		return;
	}
}

void Contexter::_StoreCand(Deserializer::KeyType k, wstring const& value)
{
	if(k.size() < 3)
		return;
	if (k[2] == L"length")
	{
		int size = _wtoi(value.c_str());
		for (int i = 0; i < size; i++)
		{
			weasel::Text t;
			m_pTarget->r_context.cinfo.candies.push_back(t);
		}		
		return;
	}
	if (k[2] == L"cursor")
	{
		int cur = _wtoi(value.c_str());
		m_pTarget->r_context.cinfo.highlighted = cur;
		return;
	}

	int idx = _wtoi(k[2].c_str());
	m_pTarget->r_context.cinfo.candies[idx].str = value;
}