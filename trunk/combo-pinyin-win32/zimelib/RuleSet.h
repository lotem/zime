// RuleSet.h
//
// ZimeLib Header
//
// Copyright (C) 2007
//
// Author: GONG Chen
// E-mail: contralisk@mail.com
// Project Home: http://zime.googlecode.com
// Licence: GPL v3
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "AbstractReader.h"

template<typename RuleType>
class RuleSet
{
public:
	RuleSet() : modif_(false) {}

	virtual ~RuleSet();

	void Add(const shared_ptr<const RuleType> p_rule);

	void Clear();

	void Load(AbstractReader* p_reader);

	typename RuleType::ObjectType Apply(typename RuleType::ObjectType const obj) const;

private:
	std::vector<shared_ptr<const RuleType> > rules_;
	bool modif_;
};

template<typename RuleType>
RuleSet<RuleType>::~RuleSet()
{
	if (!rules_.empty())
	{
		Clear();
	}
}

template<typename RuleType>
void RuleSet<RuleType>::Add( const shared_ptr<const RuleType> p_rule )
{
	rules_.push_back(p_rule);
}

template<typename RuleType>
void RuleSet<RuleType>::Clear()
{
	rules_.clear();
	modif_ = false;
}

template<typename RuleType>
void RuleSet<RuleType>::Load( AbstractReader* p_reader )
{
	Clear();

	if (p_reader->GetDirective() == L"@modif")
	{
		modif_ = true;
	}

	wstring line;
	while (
		p_reader->GetDirective() != L"@end" && 
		(line = p_reader->ReadLine()) != AbstractReader::END_OF_FILE
		)
	{
		Add(RuleType::Parse(line));
	}
}

template<typename RuleType>
typename RuleType::ObjectType 
RuleSet<RuleType>::Apply( typename RuleType::ObjectType const obj ) const
{
	if (modif_)
	{
		// modification rules
		typename RuleType::ObjectType result = obj;
		foreach (shared_ptr<const RuleType> p_rule, rules_)
		{
			result = p_rule->Apply(result);
		}
		return result;
	}
	else
	{
		// translation rules (default)
		foreach (shared_ptr<const RuleType> p_rule, rules_)
		{
			if (p_rule->ApplicableTo(obj))
			{
				return p_rule->Apply(obj);
			}
		}
		return obj;
	}
}
