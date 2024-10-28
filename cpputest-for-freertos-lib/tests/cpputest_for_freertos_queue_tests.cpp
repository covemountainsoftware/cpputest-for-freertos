/// @brief Tests of CppUTest for FreeRTOS queues.
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
#include "CppUTest/TestHarness.h"

typedef struct TestEvent {
    int32_t valueA;
    uint64_t valueB;
} TestEventT;

TEST_GROUP(QueueTests)
{
    QueueHandle_t mQueueUnderTest = nullptr;

    void setup() final
    {
    }

    void teardown() final
    {
        if (mQueueUnderTest != nullptr)
        {
            //needed for cpputest memory leak detection
            vQueueDelete(mQueueUnderTest);
            mQueueUnderTest = nullptr;
        }
    }

    void CreateUnderTest(UBaseType_t len, UBaseType_t itemSize)
    {
        mQueueUnderTest = xQueueCreate(len, itemSize);
        CHECK_TRUE(mQueueUnderTest != nullptr);
    }
};

TEST(QueueTests, can_create_a_queue)
{
    CreateUnderTest(2, sizeof(TestEventT));
}

TEST(QueueTests, can_post_to_queue)
{
    CreateUnderTest(2, sizeof(TestEventT));
    TestEventT event;
    auto rtn = xQueueSendToBack(mQueueUnderTest, &event, 1000);
    CHECK_EQUAL(pdTRUE, rtn);
}

TEST(QueueTests, posted_item_can_be_retrieved)
{
    CreateUnderTest(2, sizeof(TestEventT));
    TestEventT event = { 22, 44 };
    auto rtn = xQueueSendToBack(mQueueUnderTest, &event, 1000);
    CHECK_EQUAL(pdTRUE, rtn);

    TestEventT retrieved;
    rtn = xQueueReceive(mQueueUnderTest, &retrieved, portMAX_DELAY);
    CHECK_EQUAL(pdTRUE, rtn);

    CHECK_EQUAL(event.valueA, retrieved.valueA);
    CHECK_EQUAL(event.valueB, retrieved.valueB);
}

TEST(QueueTests, send_to_back_will_fail_if_queue_is_full)
{
    CreateUnderTest(1, sizeof(TestEventT));
    TestEventT event = { 22, 44 };
    auto rtn = xQueueSendToBack(mQueueUnderTest, &event, 1000);
    CHECK_EQUAL(pdTRUE, rtn);

    rtn = xQueueSendToBack(mQueueUnderTest, &event, 1000);
    CHECK_EQUAL(errQUEUE_FULL, rtn);
}

TEST(QueueTests, send_to_front_will_fail_if_queue_is_full)
{
    CreateUnderTest(1, sizeof(TestEventT));
    TestEventT event = { 22, 44 };
    auto rtn = xQueueSendToBack(mQueueUnderTest, &event, 1000);
    CHECK_EQUAL(pdTRUE, rtn);

    rtn = xQueueSendToFront(mQueueUnderTest, &event, 1000);
    CHECK_EQUAL(errQUEUE_FULL, rtn);
}

TEST(QueueTests, peek_will_copy_event_as_expected)
{
    CreateUnderTest(1, sizeof(TestEventT));
    TestEventT event = { 22, 44 };
    auto rtn = xQueueSendToBack(mQueueUnderTest, &event, 1000);
    CHECK_EQUAL(pdTRUE, rtn);

    TestEventT peek;
    xQueuePeek(mQueueUnderTest, &peek, 1000);
    CHECK_EQUAL(pdTRUE, rtn);
    CHECK_EQUAL(event.valueA, peek.valueA);
    CHECK_EQUAL(event.valueB, peek.valueB);

    //and we can still fully receive the same event
    TestEventT retrieved;
    rtn = xQueueReceive(mQueueUnderTest, &retrieved, portMAX_DELAY);
    CHECK_EQUAL(pdTRUE, rtn);

    CHECK_EQUAL(event.valueA, retrieved.valueA);
    CHECK_EQUAL(event.valueB, retrieved.valueB);
}

TEST(QueueTests, messages_waiting_works_as_expected)
{
    const TestEventT event = { 23, 43 };

    CreateUnderTest(3, sizeof(TestEventT));

    auto count = uxQueueMessagesWaiting(mQueueUnderTest);
    CHECK_EQUAL(0, count);

    xQueueSendToBack(mQueueUnderTest, &event, 1000);

    count = uxQueueMessagesWaiting(mQueueUnderTest);
    CHECK_EQUAL(1, count);

    xQueueSendToBack(mQueueUnderTest, &event, 1000);

    count = uxQueueMessagesWaiting(mQueueUnderTest);
    CHECK_EQUAL(2, count);
}


TEST(QueueTests, spaces_available_works_as_expected)
{
    const TestEventT event = { 23, 43 };

    CreateUnderTest(3, sizeof(TestEventT));

    auto count = uxQueueSpacesAvailable(mQueueUnderTest);
    CHECK_EQUAL(3, count);

    xQueueSendToBack(mQueueUnderTest, &event, 1000);

    count = uxQueueSpacesAvailable(mQueueUnderTest);
    CHECK_EQUAL(2, count);

    xQueueSendToBack(mQueueUnderTest, &event, 1000);

    count = uxQueueSpacesAvailable(mQueueUnderTest);
    CHECK_EQUAL(1, count);
}
