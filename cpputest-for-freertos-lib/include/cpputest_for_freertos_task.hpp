/// @brief Support methods to help with unit testing for FreeRTOS task
///        related methods.
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
#ifndef CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_TASK_HPP
#define CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_TASK_HPP

namespace cms {
    namespace test {

        /**
         * Init, prepare for task related usage, primarily for time
         * related tracking.
         */
        void TaskInit();

        /**
         * Clean up/destroy the fake CppUTest for task details, primarily
         * related to time.
         */
        void TaskDestroy();
    }
}

#endif //CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_TASK_HPP
