/// @brief Provides an implementation of a fake FreeRTOS queue set
///        cpputest-for-freertos-lib assumes that a queue set should be
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
#include "FreeRTOS.h"
#include "queue.h"

extern "C" QueueSetHandle_t xQueueCreateSet(const UBaseType_t eventQueueLength)
{
    return xQueueGenericCreate(eventQueueLength, sizeof(QueueHandle_t), queueQUEUE_TYPE_SET);
}

extern "C" BaseType_t xQueueAddToSet(QueueSetMemberHandle_t itemToAdd, QueueSetHandle_t set)
{
    auto fakeItemToAdd = static_cast<FakeQueue *>(itemToAdd);
    auto fakeSet = static_cast<FakeQueue *>(set);

    configASSERT(fakeItemToAdd != nullptr);
    configASSERT(fakeSet != nullptr);

    if ((fakeItemToAdd->queueSetContainer != nullptr) ||
        (!fakeItemToAdd->queue.empty()))
    {
        return pdFAIL;
    }
    else
    {
        fakeItemToAdd->queueSetContainer = fakeSet;
        return pdPASS;
    }
}

extern "C" BaseType_t xQueueRemoveFromSet(QueueSetMemberHandle_t itemToRemove, QueueSetHandle_t set)
{
    auto fakeItemToRemove = static_cast<FakeQueue *>(itemToRemove);

    configASSERT(fakeItemToRemove != nullptr);
    configASSERT(set != nullptr);

    if ((fakeItemToRemove->queueSetContainer != set) ||
        (!fakeItemToRemove->queue.empty()))
    {
        return pdFAIL;
    }
    else
    {
        fakeItemToRemove->queueSetContainer = nullptr;
        return pdPASS;
    }
}

extern "C" QueueSetMemberHandle_t xQueueSelectFromSet(QueueSetHandle_t queueSet, const TickType_t ticksToWait)
{
    QueueSetMemberHandle_t rtnItem = nullptr;

    auto result = xQueueReceive(queueSet, &rtnItem, ticksToWait );
    if (result == pdTRUE)
    {
        return rtnItem;
    }

    return nullptr;
}