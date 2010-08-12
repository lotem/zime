#pragma once
#include <string>

using namespace std;

class ICollectable
{
	virtual bool Collect(const string&) = 0;
};
