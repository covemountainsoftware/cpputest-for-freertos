/// @brief Provides a non-functional FreeRTOS task API
///        cpputest-for-freertos-lib assumes that unit tests
///        do not actually want threads. Instead, the unit
///        tests should test the code executed in a thread via
///        some sort of accessor. Preferably an active object
///        as demonstrated in the provided example.
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
#include "FreeRTOS.h"
#include "task.h"
#include "cpputest_freertos_timers.hpp"

namespace cms {
namespace test {

    static TickType_t s_tickCount = 0;

    void TaskInit()
    {
        s_tickCount = 0;
    }

    void TaskDestroy()
    {
        s_tickCount = 0;
    }

} //namespace test
} //namespace cms

extern "C" BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,
                        const char * const pcName,
                        const configSTACK_DEPTH_TYPE uxStackDepth,
                        void * const pvParameters,
                        UBaseType_t uxPriority,
                        TaskHandle_t * const pxCreatedTask )
{
    (void)pxTaskCode;
    (void)pcName;
    (void)uxStackDepth;
    (void)pvParameters;
    (void)uxPriority;
    (void)pxCreatedTask;
    return pdPASS;
}

extern "C" void vTaskDelete( TaskHandle_t xTaskToDelete )
{
    (void)xTaskToDelete;
}

extern "C" void vTaskDelay(const TickType_t ticks)
{
    if (cms::test::TimersIsActive())
    {
        auto duration = std::chrono::milliseconds {pdTICKS_TO_MS(ticks)};
        cms::test::MoveTimeForward(duration);
    }
    else
    {
        cms::test::s_tickCount += ticks;
    }
}

extern "C" TickType_t xTaskGetTickCount(void)
{
    if (cms::test::TimersIsActive())
    {
        auto current = cms::test::GetCurrentInternalTime();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(current);
        return pdMS_TO_TICKS(milliseconds.count());
    }
    else
    {
        return cms::test::s_tickCount;
    }
}
