// StringTransformRule.h
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

class StringTransformRule
{
public:
	// nested typename ObjectType is required by RuleSet
	typedef wstring ObjectType;

	StringTransformRule(const wstring& reg_expr, const wstring& subst);

	static const shared_ptr<const StringTransformRule> parse(const wstring& line);

	bool applicable_to(const ObjectType& obj) const;

	ObjectType apply(const ObjectType& obj) const;

private:
	boost::wregex re_;
	wstring subst_;
};
