/// @brief Support methods to help with unit testing for FreeRTOS timers.
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
#ifndef CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_TIMERS_HPP
#define CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_TIMERS_HPP

#include <chrono>

namespace cms {
namespace test {

    /**
     * Initialize the functional but fake CppUTest for FreeRTOS timers.
     */
    void TimersInit();

    /**
     * Clean up/destroy the fake CppUTest for FreeRTOS timers.
     */
    void TimersDestroy();

    /**
     * check if the current unit test has initialized the timer
     * subsystem or not.
     * @return true: yes, TimersInit() was called, so timers are active.
     */
    bool TimersIsActive();

    /**
     * Move Time Forward.
     * @param duration
     */
    void MoveTimeForward(std::chrono::nanoseconds duration);

    /**
     * Get the current time, as duration since Init was called.
     * @return
     */
    std::chrono::nanoseconds GetCurrentInternalTime();

} //namespace
}//namespace

#endif //CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_TIMERS_HPP
