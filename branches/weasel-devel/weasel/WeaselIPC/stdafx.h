// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <shellapi.h>

#pragma warning(disable : 4996)

#include <atlbase.h>
#include <atlwin.h>

#include <wtl/atlapp.h>
#include <wtl/atlframe.h>

extern CAppModule _Module;
