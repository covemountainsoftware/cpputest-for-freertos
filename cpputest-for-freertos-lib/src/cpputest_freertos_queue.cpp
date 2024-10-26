/// @brief Provides an implementation of a fake FreeRTOS queue
///        cpputest-for-freertos-lib assumes that a queue should be
///        functional, i.e. not a mock.
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

#include "cpputest_freertos_fake_queue.hpp"
#include <cstring>
#include "FreeRTOS.h"
#include "queue.h"

extern "C" QueueHandle_t xQueueGenericCreate(const UBaseType_t queueLength,
                                             const UBaseType_t itemSize,
                                             const uint8_t queueType)
{
    auto queue = new FakeQueue();
    queue->queueLength = queueLength;
    queue->itemSize = itemSize;
    queue->queueType = queueType;
    return queue;
}

extern "C" void vQueueDelete(QueueHandle_t queue)
{
    configASSERT(queue != nullptr);
    delete queue;
}

extern "C" UBaseType_t uxQueueMessagesWaiting(const QueueHandle_t queue)
{
    configASSERT(queue != nullptr);
    return queue->queue.size();
}

extern "C" UBaseType_t uxQueueSpacesAvailable(const QueueHandle_t queue)
{
    configASSERT(queue != nullptr);
    return queue->queueLength - uxQueueMessagesWaiting(queue);
}

extern "C" BaseType_t xQueueReceive(QueueHandle_t queue, void * const buffer, TickType_t ticks)
{
    configASSERT(queue != nullptr);
    (void)ticks; //in our unit testing fake, never honor ticks to wait.

    if (!queue->queue.empty())
    {
        auto front = queue->queue.front();
        memcpy(buffer, front.data(), queue->itemSize);
        queue->queue.pop_front();
        return pdTRUE;
    }
    else
    {
        return pdFALSE;
    }
}

extern "C" BaseType_t xQueueGenericSend(QueueHandle_t queue,
                                        const void * const itemToQueue,
                                        TickType_t ticks,
                                        const BaseType_t copyPosition )
{
    (void)ticks;
    configASSERT(queue != nullptr);

    if (queue->queue.size() >= queue->queueLength)
    {
        return errQUEUE_FULL;
    }

    std::vector<uint8_t> msg;
    msg.resize(queue->itemSize);
    memcpy(msg.data(), itemToQueue, queue->itemSize);

    if (copyPosition == queueSEND_TO_BACK)
    {
        queue->queue.push_back(msg);
    }
    else if (copyPosition == queueSEND_TO_FRONT)
    {
        queue->queue.push_front(msg);
    }
    else
    {
        configASSERT(true == false);
    }

    return pdTRUE;
}

extern "C" BaseType_t xQueuePeek(QueueHandle_t queue, void * const buffer, TickType_t ticks)
{
    configASSERT(queue != nullptr);
    (void)ticks; //in our unit testing fake, never honor ticks to wait.

    if (!queue->queue.empty())
    {
        auto front = queue->queue.front();
        memcpy(buffer, front.data(), queue->itemSize);
        return pdTRUE;
    }
    else
    {
        return pdFALSE;
    }
}
