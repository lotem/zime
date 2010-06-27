// WeaselIME.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "WeaselIME.h"


// This is an example of an exported variable
WEASELIME_API int nWeaselIME=0;

// This is an example of an exported function.
WEASELIME_API int fnWeaselIME(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see WeaselIME.h for the class definition
CWeaselIME::CWeaselIME()
{
	return;
}
