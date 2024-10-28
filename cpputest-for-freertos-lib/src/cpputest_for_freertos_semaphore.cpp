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

#include "cpputest_for_freertos_fake_queue.hpp"
#include <cstring>
#include "queue.h"
#include "semphr.h"

extern "C" BaseType_t xQueueSemaphoreTake(QueueHandle_t queue, TickType_t ticks)
{
    (void) ticks;  //ignore in cpputest fake semaphore
    configASSERT(queue != nullptr);
    configASSERT(queue->queueType != queueQUEUE_TYPE_RECURSIVE_MUTEX);

    return cms::InternalQueueReceive(queue);
}


extern "C" BaseType_t xQueueGiveFromISR(QueueHandle_t queue,
                                        BaseType_t * const pxHigherPriorityTaskWoken)
{
    configASSERT(queue != nullptr);
    configASSERT(queue->queueType != queueQUEUE_TYPE_RECURSIVE_MUTEX);

    (void)pxHigherPriorityTaskWoken;
    return xSemaphoreGive(queue);
}

extern "C" QueueHandle_t xQueueCreateCountingSemaphore(const UBaseType_t maxCount,
                                                       const UBaseType_t initialCount)
{
    QueueHandle_t sema = xQueueCreate(maxCount, 0);
    if (sema == nullptr)
    {
        return nullptr;
    }

    for (int i = 0; i < initialCount; ++i)
    {
        xSemaphoreGive(sema);
    }

    return sema;
}