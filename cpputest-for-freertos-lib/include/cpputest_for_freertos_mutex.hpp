/// @brief Support methods to help with unit testing for FreeRTOS mutexes.
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

#ifndef CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_MUTEX_HPP
#define CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_MUTEX_HPP

namespace cms {
namespace test {

    /**
     * Initialize the mutex state tracking,
     * such that this unit test, when Teardown is called,
     * will confirm that all mutexes are unlocked.
     */
    void MutexTrackingInit();

    /**
     * Check the state of any active mutexes.
     * If a mutex is found to be locked, then that is considered
     * a test failure. i.e. confirm that all mutex usage during
     * a unit test locks and then unlocks.
     */
    void MutexTrackingTeardown();

    /**
     * Check if any active mutexes are in a locked
     * state.
     * @return
     */
    bool IsAnyMutexLocked();

} //namespace
}//namespace

#endif //CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_MUTEX_HPP
