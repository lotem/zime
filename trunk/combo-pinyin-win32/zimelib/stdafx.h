// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once


#ifndef WINVER				// 允许使用特定于 Windows XP 或更高版本的功能。
#define WINVER 0x0501		// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用特定于 Windows 98 或更高版本的功能。
#define _WIN32_WINDOWS 0x0410 // 将此值更改为适当的值，以指定将 Windows Me 或更高版本作为目标。
#endif

#ifndef _WIN32_IE			// 允许使用特定于 IE 6.0 或更高版本的功能。
#define _WIN32_IE 0x0600	// 将此值更改为相应的值，以适用于 IE 的其他版本。
#endif

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料


// TODO: 在此处引用程序需要的其他头文件
#include <Windows.h>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <stack>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <utility>
#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/regex.hpp>
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH

using std::string;
using std::wstring;
using boost::shared_ptr;
