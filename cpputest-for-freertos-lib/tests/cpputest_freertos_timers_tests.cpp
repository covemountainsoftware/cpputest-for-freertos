/// @brief Tests of support methods to help with unit testing for FreeRTOS timers.
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
#include "timers.h"
#include "cpputest_freertos_timers.hpp"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

using namespace std::chrono_literals;

TEST_GROUP(TimersTests)
{
    void setup() final
    {
        cms::test::InitTimers();
    }

    void teardown() final
    {
        cms::test::DestroyTimers();
        mock().clear();
    }
};

TEST(TimersTests, move_time_forward_does_nothing_if_no_timers)
{
    cms::test::MoveTimeForward(10s);
    mock().checkExpectations();
}

TEST(TimersTests, move_time_forward_does_nothing_if_create_timer_but_not_started)
{
    auto timer = xTimerCreate("test", 2, pdTRUE, nullptr, [](TimerHandle_t){
        mock("TEST").actualCall("callback");
    });
    (void)timer;
    cms::test::MoveTimeForward(10s);
    mock().checkExpectations();
}

TEST(TimersTests, move_time_forward_fires_singlshot_timer_as_expected)
{
    auto timer = xTimerCreate("test", 2, pdFALSE, nullptr, [](TimerHandle_t){
        mock("TEST").actualCall("callback");
    });
    auto rtn = xTimerStart(timer, 1000);
    CHECK_EQUAL(rtn, pdPASS);
    mock("TEST").expectOneCall("callback");
    cms::test::MoveTimeForward(200s);
    mock().checkExpectations();
}

TEST(TimersTests, move_time_forward_fires_autoreload_timer_as_expected)
{
    auto timer = xTimerCreate("test", pdMS_TO_TICKS(1000), pdTRUE, nullptr, [](TimerHandle_t){
        mock("TEST").actualCall("callback");
    });
    auto rtn = xTimerStart(timer, 1000);
    CHECK_EQUAL(rtn, pdPASS);
    mock("TEST").expectNCalls(10, "callback");
    cms::test::MoveTimeForward(10s);
    mock().checkExpectations();
}

TEST(TimersTests, move_time_forward_fires_nothing_for_delete_timer)
{
    auto timer = xTimerCreate("test", pdMS_TO_TICKS(1000), pdTRUE, nullptr, [](TimerHandle_t){
        mock("TEST").actualCall("callback");
    });
    auto rtn = xTimerStart(timer, 1000);
    CHECK_EQUAL(rtn, pdPASS);
    xTimerDelete(timer, 1000);
    mock("TEST").expectNoCall("callback");
    cms::test::MoveTimeForward(10s);
    mock().checkExpectations();
}

TEST(TimersTests, move_time_forward_fires_nothing_for_start_then_stop_timer)
{
    auto timer = xTimerCreate("test", pdMS_TO_TICKS(1000), pdTRUE, nullptr, [](TimerHandle_t){
        mock("TEST").actualCall("callback");
    });
    auto rtn = xTimerStart(timer, 1000);
    CHECK_EQUAL(rtn, pdPASS);
    xTimerStop(timer, 1000);
    mock("TEST").expectNoCall("callback");
    cms::test::MoveTimeForward(10s);
    mock().checkExpectations();
}

TEST(TimersTests, move_time_forward_fires_once_then_nothing_for_start_then_stop_timer)
{
    auto timer = xTimerCreate("test", pdMS_TO_TICKS(1000), pdTRUE, nullptr, [](TimerHandle_t){
        mock("TEST").actualCall("callback");
    });
    auto rtn = xTimerStart(timer, 1000);
    CHECK_EQUAL(rtn, pdPASS);

    mock("TEST").expectOneCall("callback");
    cms::test::MoveTimeForward(1s);
    mock().checkExpectations();

    xTimerStop(timer, 1000);
    mock("TEST").expectNoCall("callback");
    cms::test::MoveTimeForward(10s);
    mock().checkExpectations();
}

TEST(TimersTests, move_time_forward_fires_timer_as_expected_after_period_change)
{
    auto timer = xTimerCreate("test", pdMS_TO_TICKS(1000), pdTRUE, nullptr, [](TimerHandle_t){
        mock("TEST").actualCall("callback");
    });
    auto rtn = xTimerStart(timer, 1000);
    CHECK_EQUAL(rtn, pdPASS);
    rtn = xTimerChangePeriod(timer, pdMS_TO_TICKS(2000), 1000);
    CHECK_EQUAL(rtn, pdPASS);
    mock("TEST").expectNCalls(5, "callback");
    cms::test::MoveTimeForward(10s);
    mock().checkExpectations();
}

TEST(TimersTests, is_timer_active_works_as_expected)
{
    auto timer = xTimerCreate("test", pdMS_TO_TICKS(1000), pdTRUE, nullptr, [](TimerHandle_t){
        mock("TEST").actualCall("callback");
    });

    auto isActive = xTimerIsTimerActive(timer);
    CHECK_EQUAL(pdFALSE, isActive);

    //start the timer
    xTimerStart(timer, 1000);
    isActive = xTimerIsTimerActive(timer);
    CHECK_EQUAL(pdTRUE, isActive);

    //stop the timer
    xTimerStop(timer, 1000);
    isActive = xTimerIsTimerActive(timer);
    CHECK_EQUAL(pdFALSE, isActive);
}

TEST(TimersTests, timer_id_works_as_expected)
{
    static uint8_t object = 1;
    auto timer = xTimerCreate("test", pdMS_TO_TICKS(1000), pdTRUE, &object, [](TimerHandle_t){
        mock("TEST").actualCall("callback");
    });

    auto getId = pvTimerGetTimerID(timer);
    CHECK_EQUAL(&object, getId);

    static uint8_t anotherObject = 2;
    vTimerSetTimerID(timer, &anotherObject);

    getId = pvTimerGetTimerID(timer);
    CHECK_EQUAL(&anotherObject, getId);
}


TEST(TimersTests, move_time_forward_fires_singleshot_timer_after_reset_as_expected)
{
    auto timer = xTimerCreate("test", pdMS_TO_TICKS(1000), pdFALSE, nullptr, [](TimerHandle_t){
        mock("TEST").actualCall("callback");
    });
    auto rtn = xTimerStart(timer, 1000);
    CHECK_EQUAL(rtn, pdPASS);

    mock("TEST").expectNoCall("callback");
    cms::test::MoveTimeForward(999ms);
    mock().checkExpectations();

    xTimerReset(timer, 1000);

    mock("TEST").expectOneCall("callback");
    cms::test::MoveTimeForward(1s);
    mock().checkExpectations();
}

TEST(TimersTests, timer_get_name_works_as_expected)
{
    auto timer = xTimerCreate("test", pdMS_TO_TICKS(1000), pdTRUE, nullptr, [](TimerHandle_t){
        mock("TEST").actualCall("callback");
    });

    auto name = pcTimerGetName(timer);
    STRCMP_EQUAL("test", name);
}

TEST(TimersTests, timer_set_reload_mode_works)
{
    auto timer = xTimerCreate("test", pdMS_TO_TICKS(1000), pdTRUE, nullptr, [](TimerHandle_t){
        mock("TEST").actualCall("callback");
    });
    auto reload = xTimerGetReloadMode(timer);
    CHECK_EQUAL(pdTRUE, reload);

    vTimerSetReloadMode(timer, pdFALSE);

    reload = xTimerGetReloadMode(timer);
    CHECK_EQUAL(pdFALSE, reload);

    xTimerStart(timer, 1000);

    mock("TEST").expectOneCall("callback");
    cms::test::MoveTimeForward(100s);
    mock().checkExpectations();
}

TEST(TimersTests, timer_get_period_works_as_expected)
{
    auto timer = xTimerCreate("test", 1111, pdTRUE, nullptr, [](TimerHandle_t){
        mock("TEST").actualCall("callback");
    });

    auto period = xTimerGetPeriod(timer);
    CHECK_EQUAL(1111, period);
}

TEST(TimersTests, timer_get_expiry_works_as_expected_with_move_time_forward_and_get_tick_count)
{
    auto timer = xTimerCreate("test", 1000, pdTRUE, nullptr, [](TimerHandle_t){
        mock("TEST").actualCall("callback");
    });

    auto rtn = xTimerStart(timer, 1000);
    CHECK_EQUAL(rtn, pdPASS);

    auto next = xTimerGetExpiryTime(timer);
    CHECK_EQUAL(1000, next);

    cms::test::MoveTimeForward(600ms);

    next = xTimerGetExpiryTime(timer);
    CHECK_EQUAL(1000, next);

    auto remaining = next - xTaskGetTickCount();
    CHECK_EQUAL(400, remaining);
}

