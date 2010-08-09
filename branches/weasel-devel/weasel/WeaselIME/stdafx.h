// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define NOIME

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <shellapi.h>
#include "Imm.h"

#pragma warning(disable : 4819)

#include <boost/format.hpp>
#include <boost/interprocess/streams/bufferstream.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#pragma warning(default : 4819)

#include <map>
#include <string>
#include <iostream>

using namespace std;
using boost::shared_ptr;
using boost::interprocess::wbufferstream;
