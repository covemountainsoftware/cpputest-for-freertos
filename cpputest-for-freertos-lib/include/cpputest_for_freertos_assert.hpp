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

#ifndef CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_ASSERT_HPP
#define CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_ASSERT_HPP

#include "FreeRTOS.h"
#include "CppUTestExt/MockSupport.h"

namespace cms {
    namespace test {

        void AssertOutputEnable();
        void AssertOutputDisable();

        static constexpr const char* ASSERT_MOCK_NAME  = "ASSERT";
        static constexpr const char* ON_ASSERT_FUNC_NAME  = "cmsAssertCalled";

        inline void MockExpectAssert()
        {
            mock(ASSERT_MOCK_NAME)
                    .expectOneCall(ON_ASSERT_FUNC_NAME)
                    .ignoreOtherParameters();
        }
    }   // namespace test
}   // namespace cms

#endif //CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_ASSERT_HPP
