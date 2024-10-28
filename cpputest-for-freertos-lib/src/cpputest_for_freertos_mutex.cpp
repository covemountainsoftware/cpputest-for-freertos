/// @brief Provides an implementation of a fake FreeRTOS semaphore
///        which, like FreeRTOS, is under the hood a queue.
///        cpputest-for-freertos-lib assumes that a queue should be
///        functional, i.e. not a mock. No blocking is implemented.
///
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

#include <list>
#include <algorithm>
#include "cpputest_for_freertos_fake_queue.hpp"
#include "cpputest_for_freertos_mutex.hpp"
#include "queue.h"
#include "semphr.h"

//must be last
#include "CppUTest/TestHarness.h"

namespace cms {
    namespace test {

        static std::list<QueueHandle_t>* s_mutexes = nullptr;

        void MutexTrackingInit()
        {
            configASSERT(s_mutexes == nullptr);
            s_mutexes = new std::list<QueueHandle_t>;
        }

        void MutexTrackingTeardown()
        {
            if (s_mutexes == nullptr)
                return;

            bool isAnyLocked = IsAnyMutexLocked();

            s_mutexes->clear();
            delete s_mutexes;
            s_mutexes = nullptr;

            if (isAnyLocked)
            {
                FAIL_TEST("A mutex is still active and locked. Test expects all mutexes to be unlocked.");
            }
        }

        bool IsAnyMutexLocked()
        {
            if (s_mutexes == nullptr)
                return false;

            return std::any_of(s_mutexes->begin(), s_mutexes->end(), [](QueueDefinition* mutex)
            {
                return uxSemaphoreGetCount(mutex) == 0;
            });
        }

        void MutexAboutToDelete(QueueHandle_t mutex)
        {
            if (s_mutexes == nullptr)
                return;

            s_mutexes->remove(mutex);
        }
    } //namespace
}//namespace

extern "C" QueueHandle_t xQueueCreateMutex( const uint8_t ucQueueType )
{
    (void)ucQueueType;
    auto mutex = xQueueGenericCreate(1, 0, ucQueueType);
    switch (ucQueueType) {
        case queueQUEUE_TYPE_MUTEX:
            //wasn't documented, but in experiment, the standard mutex is created unlocked
            //(i.e) with one token available
            xSemaphoreGive(mutex);
            break;
        case queueQUEUE_TYPE_RECURSIVE_MUTEX:
            //todo
            break;
        default:
            configASSERT(true == false);
            break;
    }
    configASSERT(mutex != nullptr);
    if (cms::test::s_mutexes != nullptr)
    {
        cms::test::s_mutexes->push_back(mutex);
    }
    return mutex;
}