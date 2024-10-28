/// @brief Support methods to help with unit testing for FreeRTOS configASSERT.
/// @ingroup
/// @cond
///***************************************************************************
///
/// Copyright (C) 2024 Matthew Eshleman. All rights reserved.
///
/// This program is open source software: you can redistribute it and/or
/// modify it under the terms of the GNU General Public License as published
/// by the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
///
/// Alternatively, upon written permission from Matthew Eshleman, this program
/// may be distributed and modified under the terms of a Commercial
/// License. For further details, see the Contact Information below.
///
/// Contact Information:
///   Matthew Eshleman
///   https://covemountainsoftware.com
///   info@covemountainsoftware.com
///***************************************************************************
/// @endcond

#include <cstdio>
#include "cpputest_for_freertos_assert.hpp"

static bool m_printAssert = true;

void cms::test::AssertOutputEnable()
{
    m_printAssert = true;
}

void cms::test::AssertOutputDisable()
{
    m_printAssert = false;
}

extern "C" void cmsAssertCalled( const char * file, unsigned long line )
{
    if (m_printAssert)
    {
        fprintf(stdout, "\n%s(%s:%lu)\n", __FUNCTION__ , file, line);
    }

    // The TEST_EXIT macro used below is throwing an exception.
    // If any code being tested is C++ and marked noexcept, then
    // sadly the following applies:
    //
    // Per https://en.cppreference.com/w/cpp/language/noexcept_spec:
    //   "Non-throwing functions are permitted to call potentially-throwing
    //    functions. Whenever an exception is thrown and the search for a
    //    handler encounters the outermost block of a non-throwing function,
    //    the function std::terminate ... is called ..."
    //
    mock(cms::test::ASSERT_MOCK_NAME)
            .actualCall(cms::test::ON_ASSERT_FUNC_NAME)
            .withParameter("file", file)
            .withParameter("line", line);

    TEST_EXIT;
}
