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

	static const shared_ptr<const StringTransformRule> Parse(const wstring& line);

	bool ApplicableTo(const ObjectType& obj) const;

	ObjectType Apply(const ObjectType& obj) const;

private:
	boost::wregex re_;
	wstring subst_;
};
