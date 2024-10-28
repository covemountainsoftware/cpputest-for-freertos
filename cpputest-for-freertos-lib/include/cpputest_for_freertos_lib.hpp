/// @brief Top level init/teardown methods for CppUTest for FreeRTOS library.
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

#ifndef CPPUTEST_FOR_FREERTOS_LIB_HPP
#define CPPUTEST_FOR_FREERTOS_LIB_HPP

#include "cpputest_for_freertos_assert.hpp"
#include "cpputest_for_freertos_task.hpp"
#include "cpputest_for_freertos_timers.hpp"

namespace cms {
    namespace test {
        /**
         * call this in your unit test setup() method to initialize
         * all available CppUTest for FreeRTOS modules.
         */
        void LibInitAll() {
            TaskInit();
            AssertOutputEnable();
            TimersInit();
        }

        /**
         * call this in your unit test teardown() method to correctly
         * destroy/teardown all available CppUTest for FreeRTOS modules.
         */
        void LibTeardownAll() {
            TimersDestroy();
            TaskDestroy();
        }
    } // namespace test
} //namespace cms

#endif //CPPUTEST_FOR_FREERTOS_LIB_HPP
