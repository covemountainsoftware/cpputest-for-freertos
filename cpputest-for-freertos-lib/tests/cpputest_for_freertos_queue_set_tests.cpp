/// @brief Tests of CppUTest for FreeRTOS queue sets.
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
#include "queue.h"
#include "semphr.h"
#include "cpputest_for_freertos_memory.hpp"

//must be last
#include "CppUTest/TestHarness.h"

TEST_GROUP(QueueSetTests)
{
    cms::test::unique_queue mUnderTest = nullptr;

    void setup() final
    {
    }

    void teardown() final
    {
    }

    void CreateSetUnderTest(UBaseType_t length)
    {
        auto set = xQueueCreateSet(length);
        mUnderTest = cms::test::unique_queue(set);
        CHECK_TRUE(mUnderTest != nullptr);
    }

    static cms::test::unique_queue CreateQueue(UBaseType_t len, UBaseType_t itemSize)
    {
        auto rtn = xQueueCreate(len, itemSize);
        CHECK_TRUE(rtn != nullptr);
        cms::test::unique_queue queue(rtn);
        return queue;
    }

    cms::test::unique_queue CreateQueueAndAddToUnderTestSet(UBaseType_t len, UBaseType_t itemSize) const
    {
        auto created = xQueueCreate(len, itemSize);
        CHECK_TRUE(created != nullptr);

        auto rtn = xQueueAddToSet(created, mUnderTest.get());
        CHECK_EQUAL(pdPASS, rtn);

        cms::test::unique_queue queue(created);
        return queue;
    }
};

TEST(QueueSetTests, can_create_a_queue_set)
{
    CreateSetUnderTest(2);
}

TEST(QueueSetTests, can_add_a_queue_to_a_set_and_remove_from_set)
{
    CreateSetUnderTest(2);

    auto queue = CreateQueue(2, 2);
    auto rtn = xQueueAddToSet(queue.get(), mUnderTest.get());
    CHECK_EQUAL(pdPASS, rtn);

    rtn = xQueueRemoveFromSet(queue.get(), mUnderTest.get());
    CHECK_EQUAL(pdPASS, rtn);
}

TEST(QueueSetTests, adding_a_queue_with_events_to_a_set_fails)
{
    const int testValue = 234;
    CreateSetUnderTest(2);
    auto queue = CreateQueue(2, sizeof(int));
    auto rtn = xQueueSendToBack(queue.get(), &testValue, portMAX_DELAY);
    CHECK_EQUAL(pdTRUE, rtn);

    rtn = xQueueAddToSet(queue.get(), mUnderTest.get());
    CHECK_EQUAL(pdFAIL, rtn);
}

TEST(QueueSetTests, adding_a_queue_already_added_to_a_set_fails)
{
    CreateSetUnderTest(2);
    auto queue = CreateQueue(2, sizeof(int));
    auto rtn = xQueueAddToSet(queue.get(), mUnderTest.get());
    CHECK_EQUAL(pdPASS, rtn);

    //add it again should fail
    rtn = xQueueAddToSet(queue.get(), mUnderTest.get());
    CHECK_EQUAL(pdFAIL, rtn);
}

TEST(QueueSetTests, can_add_a_queue_to_a_set_and_fails_to_remove_from_another_set)
{
    CreateSetUnderTest(2);
    cms::test::unique_queue otherSet(xQueueCreateSet(2));
    CHECK_TRUE(otherSet != nullptr);

    auto queue = CreateQueueAndAddToUnderTestSet(2, 2);

    auto rtn = xQueueRemoveFromSet(queue.get(), otherSet.get());
    CHECK_EQUAL(pdFAIL, rtn);
}

TEST(QueueSetTests, select_from_set_will_not_block_and_return_null_if_no_events)
{
    CreateSetUnderTest(2);
    auto queue = CreateQueueAndAddToUnderTestSet(2, sizeof(int));

    auto selectResult = xQueueSelectFromSet(mUnderTest.get(), portMAX_DELAY);
    CHECK_EQUAL(nullptr, selectResult);
}

TEST(QueueSetTests, select_from_set_will_return_expected_queue_with_event)
{
    const uint16_t testValue = 4321;

    CreateSetUnderTest(2);
    auto queue = CreateQueueAndAddToUnderTestSet(2, sizeof(testValue));

    auto rtn = xQueueSendToBack(queue.get(), &testValue, 1000);
    CHECK_EQUAL(pdTRUE, rtn);

    auto selectResult = xQueueSelectFromSet(mUnderTest.get(), portMAX_DELAY);
    CHECK_TRUE(selectResult != nullptr);
    CHECK_EQUAL(queue.get(), selectResult);

    uint16_t receivedValue = 0;
    rtn = xQueueReceive(selectResult, &receivedValue, portMAX_DELAY);
    CHECK_EQUAL(pdTRUE, rtn);
    CHECK_EQUAL(testValue, receivedValue);

    //confirm select shows null now
    selectResult = xQueueSelectFromSet(mUnderTest.get(), portMAX_DELAY);
    CHECK_EQUAL(nullptr, selectResult);
}

TEST(QueueSetTests, select_from_set_with_multiple_queues_will_return_expected_queues_in_expected_order)
{
    const uint16_t testValueQueue1 = 4321; //queue1: 16bit
    const int32_t testValueQueue2 = 9876;  //queue2: 32bit

    CreateSetUnderTest(4);
    auto queue1 = CreateQueueAndAddToUnderTestSet(2, sizeof(testValueQueue1));
    auto queue2 = CreateQueueAndAddToUnderTestSet(2, sizeof(testValueQueue2));

    //send via queue2, then queue1
    auto rtn = xQueueSendToBack(queue2.get(), &testValueQueue2, portMAX_DELAY);
    CHECK_EQUAL(pdTRUE, rtn);
    rtn = xQueueSendToBack(queue1.get(), &testValueQueue1, portMAX_DELAY);
    CHECK_EQUAL(pdTRUE, rtn);

    auto selectResultFirst = xQueueSelectFromSet(mUnderTest.get(), portMAX_DELAY);
    CHECK_TRUE(selectResultFirst != nullptr);
    CHECK_EQUAL(queue2.get(), selectResultFirst);

    auto selectResultSecond = xQueueSelectFromSet(mUnderTest.get(), portMAX_DELAY);
    CHECK_TRUE(selectResultSecond != nullptr);
    CHECK_EQUAL(queue1.get(), selectResultSecond);

    //go ahead and confirm queue2 is as expected
    int32_t receivedIntValue = 0;
    rtn = xQueueReceive(selectResultFirst, &receivedIntValue, portMAX_DELAY);
    CHECK_EQUAL(pdTRUE, rtn);
    CHECK_EQUAL(testValueQueue2, receivedIntValue);

    //go ahead and confirm queue1 is as expected
    uint16_t receivedShortValue = 0;
    rtn = xQueueReceive(selectResultSecond, &receivedShortValue, portMAX_DELAY);
    CHECK_EQUAL(pdTRUE, rtn);
    CHECK_EQUAL(testValueQueue1, receivedShortValue);

    //confirm select shows null now
    auto selectResult = xQueueSelectFromSet(mUnderTest.get(), portMAX_DELAY);
    CHECK_EQUAL(nullptr, selectResult);
}

TEST(QueueSetTests, select_from_set_will_return_expected_semaphore)
{
    CreateSetUnderTest(2);
    cms::test::unique_sema sema (xSemaphoreCreateBinary());
    CHECK_TRUE(sema != nullptr);

    auto rtn = xQueueAddToSet(sema.get(), mUnderTest.get());
    CHECK_EQUAL(pdPASS, rtn);

    auto selectResult = xQueueSelectFromSet(mUnderTest.get(), portMAX_DELAY);
    CHECK_EQUAL(nullptr, selectResult);

    xSemaphoreGive(sema.get());

    selectResult = xQueueSelectFromSet(mUnderTest.get(), portMAX_DELAY);
    CHECK_EQUAL(sema.get(), selectResult);

    auto count = uxSemaphoreGetCount(selectResult);
    CHECK_EQUAL(1, count);
}

TEST(QueueSetTests, select_from_set_with_multiple_semaphores_will_return_expected_semaphore_in_expected_order)
{
    CreateSetUnderTest(2);
    cms::test::unique_sema sema1 (xSemaphoreCreateBinary());
    CHECK_TRUE(sema1 != nullptr);
    cms::test::unique_sema sema2 (xSemaphoreCreateBinary());
    CHECK_TRUE(sema2 != nullptr);
    auto rtn = xQueueAddToSet(sema1.get(), mUnderTest.get());
    CHECK_EQUAL(pdPASS, rtn);
    rtn = xQueueAddToSet(sema2.get(), mUnderTest.get());
    CHECK_EQUAL(pdPASS, rtn);

    auto selectResult = xQueueSelectFromSet(mUnderTest.get(), portMAX_DELAY);
    CHECK_EQUAL(nullptr, selectResult);

    xSemaphoreGive(sema2.get());
    xSemaphoreGive(sema1.get());

    selectResult = xQueueSelectFromSet(mUnderTest.get(), portMAX_DELAY);
    CHECK_EQUAL(sema2.get(), selectResult);
    auto count = uxSemaphoreGetCount(selectResult);
    CHECK_EQUAL(1, count);

    selectResult = xQueueSelectFromSet(mUnderTest.get(), portMAX_DELAY);
    CHECK_EQUAL(sema1.get(), selectResult);
    count = uxSemaphoreGetCount(selectResult);
    CHECK_EQUAL(1, count);
}