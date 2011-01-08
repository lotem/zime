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

	void add(const shared_ptr<const RuleType> p_rule);

	void clear();

	void load(AbstractReader* p_reader);

	typename RuleType::ObjectType apply(typename RuleType::ObjectType const obj) const;

private:
	std::vector<shared_ptr<const RuleType> > rules_;
	bool modif_;
};

template<typename RuleType>
RuleSet<RuleType>::~RuleSet()
{
	if (!rules_.empty())
	{
		clear();
	}
}

template<typename RuleType>
void RuleSet<RuleType>::add( const shared_ptr<const RuleType> p_rule )
{
	rules_.push_back(p_rule);
}

template<typename RuleType>
void RuleSet<RuleType>::clear()
{
	rules_.clear();
	modif_ = false;
}

template<typename RuleType>
void RuleSet<RuleType>::load( AbstractReader* p_reader )
{
	clear();

	if (p_reader->get_directive() == L"@modif")
	{
		modif_ = true;
	}

	wstring line;
	while (
		p_reader->get_directive() != L"@end" && 
		(line = p_reader->read_line()) != AbstractReader::END_OF_FILE
		)
	{
		add(RuleType::parse(line));
	}
}

template<typename RuleType>
typename RuleType::ObjectType 
RuleSet<RuleType>::apply( typename RuleType::ObjectType const obj ) const
{
	if (modif_)
	{
		// modification rules
		typename RuleType::ObjectType result = obj;
		foreach (shared_ptr<const RuleType> p_rule, rules_)
		{
			result = p_rule->apply(result);
		}
		return result;
	}
	else
	{
		// translation rules (default)
		foreach (shared_ptr<const RuleType> p_rule, rules_)
		{
			if (p_rule->applicable_to(obj))
			{
				return p_rule->apply(obj);
			}
		}
		return obj;
	}
}
