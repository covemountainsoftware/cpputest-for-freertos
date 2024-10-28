/// @brief Tests of CppUTest FreeRTOS task methods.
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
#include "cpputest_for_freertos_timers.hpp"
#include "cpputest_for_freertos_task.hpp"
#include "CppUTest/TestHarness.h"

TEST_GROUP(TaskTests)
{
    void setup() final
    {
        cms::test::TaskInit();
    }

    void teardown() final
    {
        cms::test::TaskDestroy();
    }
};

TEST(TaskTests, task_delay_method_is_available_and_tracks_time_when_not_using_timers)
{
    auto count1 = xTaskGetTickCount();
    vTaskDelay(100);
    auto count2 = xTaskGetTickCount();
    CHECK_EQUAL(100, count2 - count1);
}

TEST(TaskTests, task_delay_method_will_move_timers_time_forward_if_timers_are_active)
{
    cms::test::TimersInit();
    auto count1 = xTaskGetTickCount();
    vTaskDelay(100);
    auto count2 = xTaskGetTickCount();
    CHECK_EQUAL(100, count2 - count1);
    auto fromTimers = cms::test::GetCurrentInternalTime();

    CHECK_TRUE(std::chrono::milliseconds(pdTICKS_TO_MS(100)) == fromTimers);
    cms::test::TimersDestroy();
}
