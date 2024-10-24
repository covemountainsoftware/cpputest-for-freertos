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

#include <deque>
#include <vector>
#include <cstring>
#include "FreeRTOS.h"
#include "queue.h"

typedef struct QueueDefinition
{
    UBaseType_t queueLength = {};
    UBaseType_t itemSize = {};
    uint8_t queueType = {};
    std::deque<std::vector<uint8_t>> queue = {};
} FakeQueue;

QueueHandle_t xQueueGenericCreate( const UBaseType_t uxQueueLength,
                                   const UBaseType_t uxItemSize,
                                   const uint8_t ucQueueType )
{
    auto queue = new FakeQueue();
    queue->queueLength = uxQueueLength;
    queue->itemSize = uxItemSize;
    queue->queueType = ucQueueType;
    return queue;
}

void vQueueDelete( QueueHandle_t xQueue )
{
    configASSERT(xQueue != nullptr);
    delete xQueue;
}

UBaseType_t uxQueueMessagesWaiting( const QueueHandle_t xQueue )
{
    configASSERT(xQueue != nullptr);
    return xQueue->queue.size();
}

BaseType_t xQueueReceive( QueueHandle_t xQueue,
                          void * const pvBuffer,
                          TickType_t xTicksToWait )
{
    configASSERT(xQueue != nullptr);
    (void)xTicksToWait; //in our unit testing fake, never honor ticks to wait.

    if (!xQueue->queue.empty())
    {
        auto front = xQueue->queue.front();
        memcpy(pvBuffer, front.data(), xQueue->itemSize);
        xQueue->queue.pop_front();
        return pdTRUE;
    }
    else
    {
        return pdFALSE;
    }
}

/*
* @param xCopyPosition Can take the value queueSEND_TO_BACK to place the
        * item at the back of the queue, or queueSEND_TO_FRONT to place the item
* at the front of the queue (for high priority messages).
 */
BaseType_t xQueueGenericSend( QueueHandle_t xQueue,
                              const void * const pvItemToQueue,
                              TickType_t xTicksToWait,
                              const BaseType_t xCopyPosition )
{
    (void)xTicksToWait;
    configASSERT(xQueue != nullptr);

    if (xQueue->queue.size() >= xQueue->queueLength)
    {
        return errQUEUE_FULL;
    }

    std::vector<uint8_t> msg;
    msg.resize(xQueue->itemSize);
    memcpy(msg.data(), pvItemToQueue, xQueue->itemSize);

    if (xCopyPosition == queueSEND_TO_BACK)
    {
        xQueue->queue.push_back(msg);
    }
    else if (xCopyPosition == queueSEND_TO_FRONT)
    {
        xQueue->queue.push_front(msg);
    }
    else
    {
        configASSERT(true == false);
    }

    return pdTRUE;
}
