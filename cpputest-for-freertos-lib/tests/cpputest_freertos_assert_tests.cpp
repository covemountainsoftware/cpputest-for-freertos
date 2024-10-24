/// @brief Tests of support methods to help with unit testing for FreeRTOS configASSERT.
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
#include "CppUTest/TestHarness.h"
#include "cpputest_freertos_assert.hpp"

TEST_GROUP(AssertTests)
{
    void setup() final
    {
    }

    void teardown() final
    {
        mock().clear();
    }
};

TEST(AssertTests, configASSERT_results_in_expected_mock_hit_and_proper_test_exit)
{
    cms::test::AssertOutputDisable();
    cms::test::MockExpectAssert();
    configASSERT(true == false);
    mock().checkExpectations();
}
