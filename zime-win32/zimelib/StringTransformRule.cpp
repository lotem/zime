// StringTransformRule.cpp
//
// ZimeLib Source
//
// Copyright (C) 2007
//
// Author: GONG Chen
// E-mail: contralisk@mail.com
// Project Home: http://zime.googlecode.com
// Licence: GPL v3
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StringTransformRule.h"


StringTransformRule::StringTransformRule(const wstring& reg_expr, const wstring& subst)
	: re_(reg_expr), subst_(subst) 
{
}

const shared_ptr<const StringTransformRule> StringTransformRule::Parse( const wstring& line )
{
	std::vector<wstring> v;
	boost::algorithm::split(v, line, boost::algorithm::is_space());
	assert(v.size() == 2);
	boost::algorithm::replace_all(v[0], "_", " ");
	boost::algorithm::replace_all(v[1], "_", " ");
	return shared_ptr<const StringTransformRule>(new StringTransformRule(v[0], v[1]));
}

StringTransformRule::ObjectType StringTransformRule::Apply( const ObjectType& obj ) const
{
	return boost::regex_replace(obj, re_, subst_);
}

bool StringTransformRule::ApplicableTo( const ObjectType& obj ) const
{
	return boost::regex_match(obj, re_);
}
