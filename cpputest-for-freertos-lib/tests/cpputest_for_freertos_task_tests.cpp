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

TEST(TaskTests, task_delay_until_method_is_available)
{
    auto lastWakeTime = xTaskGetTickCount();
    auto count1 = lastWakeTime;
    vTaskDelayUntil(&lastWakeTime, 10);
    auto count2 = xTaskGetTickCount();
    CHECK_EQUAL(10, count2 - count1);
}

TEST(TaskTests, task_delay_until_method_returns_false_if_next_is_already_happened)
{
    auto lastWakeTime = xTaskGetTickCount();

    //simulate a task switch which means the next delay until deadline has passed
    vTaskDelay(11);

    auto didSleep = xTaskDelayUntil(&lastWakeTime, 10);
    CHECK_EQUAL(pdFALSE, didSleep);
}

TEST(TaskTests, task_delay_until_method_returns_true_if_sleep_was_needed)
{
    auto lastWakeTime = xTaskGetTickCount();
    auto count1 = lastWakeTime;

    //simulate a brief task switch, but the next deadline is still in the future
    vTaskDelay(3);

    auto didSleep = xTaskDelayUntil(&lastWakeTime, 10);
    CHECK_EQUAL(pdTRUE, didSleep);

    auto count2 = xTaskGetTickCount();
    CHECK_EQUAL(10, count2 - count1);
}

static void staticTaskCode(void * parameters)
{
    (void)parameters;
    //for testing, does nothing
}

TEST(TaskTests, task_create_static_is_available)
{
    static StaticTask_t staticTaskBuffer;
    static std::array<StackType_t, 200> staticStack;

    auto taskHandle = xTaskCreateStatic(
            staticTaskCode,       /* Function that implements the task. */
            "TEST",          /* Text name for the task. */
            staticStack.size(),      /* Number of indexes in the xStack array. */
            ( void * ) 1,    /* Parameter passed into the task. */
            tskIDLE_PRIORITY,/* Priority at which the task is created. */
            staticStack.data(),          /* Array to use as the task's stack. */
            &staticTaskBuffer );  /* Variable to hold the task's data structure. */

    CHECK_TRUE(taskHandle != nullptr);
}