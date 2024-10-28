/// @brief Tests of CppUTest for FreeRTOS mutex implementation
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
#include "semphr.h"
#include "cpputest_for_freertos_mutex.hpp"
#include "CppUTest/TestHarness.h"


TEST_GROUP(MutexTests)
{
    SemaphoreHandle_t mMutexUnderTest;

    void setup() final
    {
    }

    void teardown() final
    {
        if (mMutexUnderTest != nullptr)
        {
            vSemaphoreDelete(mMutexUnderTest);
            mMutexUnderTest = nullptr;
        }
    }

    void CreateMutex()
    {
        mMutexUnderTest = xSemaphoreCreateMutex();
        CHECK_TRUE(nullptr != mMutexUnderTest);
    }

    void CreateRecursiveMutex()
    {
        mMutexUnderTest = xSemaphoreCreateRecursiveMutex();
        CHECK_TRUE(nullptr != mMutexUnderTest);
    }
};

TEST(MutexTests, can_create_a_mutex_and_is_not_locked)
{
    CreateMutex();
    CHECK_EQUAL(1, uxSemaphoreGetCount(mMutexUnderTest));
}

TEST(MutexTests, can_lock_and_unlock_a_mutex)
{
    CreateMutex();
    xSemaphoreTake(mMutexUnderTest, 1000);
    CHECK_EQUAL(0, uxSemaphoreGetCount(mMutexUnderTest));
    xSemaphoreGive(mMutexUnderTest);
    CHECK_EQUAL(1, uxSemaphoreGetCount(mMutexUnderTest));
}

TEST(MutexTests, library_can_detect_locked_mutexes)
{
    CHECK_FALSE(cms::test::IsAnyMutexLocked());
    cms::test::MutexTrackingInit();
    CreateMutex();
    CHECK_FALSE(cms::test::IsAnyMutexLocked());

    xSemaphoreTake(mMutexUnderTest, 1000);
    CHECK_TRUE(cms::test::IsAnyMutexLocked());

    xSemaphoreGive(mMutexUnderTest);
    CHECK_FALSE(cms::test::IsAnyMutexLocked());
    cms::test::MutexTrackingTeardown();
}

TEST(MutexTests, library_does_not_detect_locked_mutex_that_was_deleted)
{
    cms::test::MutexTrackingInit();
    CreateMutex();
    xSemaphoreTake(mMutexUnderTest, 1000);
    CHECK_TRUE(cms::test::IsAnyMutexLocked());

    vSemaphoreDelete(mMutexUnderTest);
    mMutexUnderTest = nullptr;
    CHECK_FALSE(cms::test::IsAnyMutexLocked());
    cms::test::MutexTrackingTeardown();
}

TEST(MutexTests, can_create_a_recursive_mutex)
{
    CreateRecursiveMutex();
    CHECK_EQUAL(1, uxSemaphoreGetCount(mMutexUnderTest));
}

TEST(MutexTests, can_lock_a_recursive_mutex_multiple_times)
{
    CreateRecursiveMutex();
    CHECK_EQUAL(1, uxSemaphoreGetCount(mMutexUnderTest));
    CHECK_EQUAL(pdTRUE, xSemaphoreTakeRecursive(mMutexUnderTest, 1000));
    CHECK_EQUAL(pdTRUE, xSemaphoreTakeRecursive(mMutexUnderTest, 1000));
    CHECK_EQUAL(pdTRUE, xSemaphoreTakeRecursive(mMutexUnderTest, 1000));
    CHECK_EQUAL(0, uxSemaphoreGetCount(mMutexUnderTest));

    //now confirm give recursive while here
    CHECK_EQUAL(pdTRUE, xSemaphoreGiveRecursive(mMutexUnderTest));
    CHECK_EQUAL(0, uxSemaphoreGetCount(mMutexUnderTest));
    CHECK_EQUAL(pdTRUE, xSemaphoreGiveRecursive(mMutexUnderTest));
    CHECK_EQUAL(0, uxSemaphoreGetCount(mMutexUnderTest));
    CHECK_EQUAL(pdTRUE, xSemaphoreGiveRecursive(mMutexUnderTest));
    CHECK_EQUAL(1, uxSemaphoreGetCount(mMutexUnderTest));
}

TEST(MutexTests, library_can_detect_locked_recursive_mutexes)
{
    CHECK_FALSE(cms::test::IsAnyMutexLocked());
    cms::test::MutexTrackingInit();
    CreateRecursiveMutex();
    CHECK_FALSE(cms::test::IsAnyMutexLocked());

    xSemaphoreTakeRecursive(mMutexUnderTest, 1000);
    CHECK_TRUE(cms::test::IsAnyMutexLocked());

    xSemaphoreGiveRecursive(mMutexUnderTest);
    CHECK_FALSE(cms::test::IsAnyMutexLocked());
    cms::test::MutexTrackingTeardown();
}

TEST(MutexTests, library_does_not_detect_locked_recursive_mutex_that_was_deleted)
{
    cms::test::MutexTrackingInit();
    CreateRecursiveMutex();
    xSemaphoreTakeRecursive(mMutexUnderTest, 1000);
    CHECK_TRUE(cms::test::IsAnyMutexLocked());

    vSemaphoreDelete(mMutexUnderTest);
    mMutexUnderTest = nullptr;
    CHECK_FALSE(cms::test::IsAnyMutexLocked());
    cms::test::MutexTrackingTeardown();
}