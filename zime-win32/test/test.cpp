// test.cpp
//
////////////////////////////////////////////////////////////////////// 
#include "stdafx.h"
#include "../zimelib/KeyboardHook.h"
#include "../zimelib/Timer.h"

#define BOOST_TEST_MODULE "zime tests"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE( zimlib );

BOOST_AUTO_TEST_CASE( non_sense )
{
	BOOST_CHECK(1 != 0);
}

BOOST_AUTO_TEST_CASE( testTimer )
{
	// only the first call to Timer::Create actually creates the timer
	Timer& timer1 = Timer::Create((HWND)1, (UINT)2);
	BOOST_CHECK_EQUAL(2, timer1.GetTimerId());
	Timer& timer2 = Timer::Create((HWND)3, (UINT)4);
	BOOST_CHECK_EQUAL(2, timer2.GetTimerId());
}

BOOST_AUTO_TEST_SUITE_END();
