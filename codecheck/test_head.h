#pragma once

//////////////////////////////////////////////////////////////////////////

#ifndef CODECHECK_CUSTOM_STROUT

#include <iostream>
using namespace std;

#define strout cout

#endif/*CODECHECK_CUSTOM_STROUT*/

//////////////////////////////////////////////////////////////////////////

#define TEST_FUNCTION(...) \
    strout << endl \
           << "==================================" << endl \
           << "\t" __VA_ARGS__ << __FUNCTION__ << endl \
           << "==================================" << endl

#define TEST_CASE(...) \
    strout << endl \
           << "----------------------------------" << endl \
           << "\t" __VA_ARGS__ << __FUNCTION__ << " ->: " << endl \
           << "----------------------------------" << endl

//////////////////////////////////////////////////////////////////////////

#include "test_preprocessor.h"
#include "test_typemanip.h"
#include "test_algorithm.h"
#include "test_utility.h"
#include "test_time.h"
#include "test_delegate.h"
#include "test_thread.h"
#include "test_memory.h"
#include "test_finalizer.h"

//////////////////////////////////////////////////////////////////////////

void testAll(void)
{
    //testPreprocessor();
    //testTypeManip();
    //testAlgorithm();
    //testUtility();
    //testTime();
    //testDelegate();
    //testFinalizer();
    //testThread();
    testMemory();

    TEST_FUNCTION(<< "Finished ");
}