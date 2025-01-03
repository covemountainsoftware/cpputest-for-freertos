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

#include "cpputest_for_freertos_fake_queue.hpp"
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

namespace cms{
    namespace test {
        extern void MutexAboutToDelete(QueueHandle_t mutex);
    }
}
extern "C" void vQueueDelete(QueueHandle_t queue)
{
    configASSERT(queue != nullptr);
    if (queue->queueType == queueQUEUE_TYPE_MUTEX)
    {
        cms::test::MutexAboutToDelete(queue);
    }
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

BaseType_t cms::InternalQueueReceive(FakeQueue * queue)
{
    configASSERT(queue != nullptr);

    uint64_t dummy;
    return cms::InternalQueueReceive(queue, &dummy);
}

BaseType_t cms::InternalQueueReceive(FakeQueue * queue, void * const buffer)
{
    configASSERT(queue != nullptr);

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

extern "C" BaseType_t xQueueReceive(QueueHandle_t queue, void * const buffer, TickType_t ticks)
{
    configASSERT(queue != nullptr);
    configASSERT(buffer != nullptr);

    (void)ticks; //in our unit testing fake, never honor ticks to wait.
    return cms::InternalQueueReceive(queue, buffer);
}

extern "C" BaseType_t xQueueGenericSend(QueueHandle_t queue,
                                        const void * const itemToQueue,
                                        TickType_t ticks,
                                        const BaseType_t copyPosition)
{
    (void)ticks;
    configASSERT(queue != nullptr);
    configASSERT(!((itemToQueue == nullptr) && (queue->itemSize != 0U)));
    configASSERT(!((copyPosition == queueOVERWRITE) && (queue->queueLength != 1)));

    if ((copyPosition != queueOVERWRITE) &&
        (queue->queue.size() >= queue->queueLength))
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
    else if (copyPosition == queueOVERWRITE)
    {
        queue->queue.pop_front();
        queue->queue.push_front(msg);
    }
    else
    {
        configASSERT(true == false);
    }

    if (queue->queueSetContainer != nullptr)
    {
        xQueueGenericSend(queue->queueSetContainer, &queue, ticks, queueSEND_TO_BACK);
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

extern "C" QueueHandle_t xQueueGenericCreateStatic(const UBaseType_t queueLength,
                                        const UBaseType_t itemSize,
                                        uint8_t * queueStorage,
                                        StaticQueue_t * staticQueue,
                                        const uint8_t queueType)
{
    //for unit testing, just going to ignore the provided static queue and allocate
    //dynamically.
    (void)queueStorage;
    (void)staticQueue;
    auto queue = xQueueGenericCreate(queueLength, itemSize, queueType);
    return queue;
}

extern "C" void vQueueAddToRegistry(QueueHandle_t queue, const char * queueName)
{
    configASSERT(queue != nullptr);
    configASSERT(queueName != nullptr);
    queue->registryName = queueName;
}

extern "C" const char * pcQueueGetName(QueueHandle_t queue)
{
    configASSERT(queue != nullptr);
    return queue->registryName;
}

extern "C" void vQueueUnregisterQueue(QueueHandle_t queue)
{
    configASSERT(queue != nullptr);
    queue->registryName = nullptr;
}