/*
MIT License

Copyright (c) <2019-2020> <Matthew Eshleman - https://covemountainsoftware.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "hwLockCtrlService.h"
#include "cpputest_for_freertos_lib.hpp"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "hwLockCtrl.h"

static constexpr const char* HW_LOCK_CTRL_MOCK = "HwLockCtrl";
static constexpr const char* CB_MOCK = "TestCb";

using namespace std::chrono_literals;

static void TestLockStateCallback(HLCS_LockStateT state)
{
    mock(CB_MOCK).actualCall("LockStateCallback").withIntParameter("state", static_cast<int>(state));
}

static void TestSelfTestResultCallback(HLCS_SelfTestResultT result)
{
    mock(CB_MOCK).actualCall("SelfTestResultCallback").withIntParameter("result", static_cast<int>(result));
}

/**
 * @brief This test demonstrates the following key points:
 *         1) Does NOT test the thread associated with the active object,
 *            rather, tests the behavior of the active object via a back door
 *            to the active object's event queue.
 *         2) Tests the internal state machine of the active object without
 *            internal knowledge of the state machine. Rather, only
 *            by observing the behavior and associated output/results.
 *         3) Create helper methods to drive the internal "state" to known
 *            starting points.
 *         4) Continue to follow software engineering best practices, such
 *            as adhering to the DRY principle.
 */
TEST_GROUP(HwLockCtrlServiceTests)
{
    void setup() final
    {
        cms::test::LibInitAll();
        HLCS_Init();
        HLCS_RegisterChangeStateCallback(TestLockStateCallback);
        HLCS_RegisterSelfTestResultCallback(TestSelfTestResultCallback);
    }

    void teardown() final
    {
        HLCS_Destroy(); //ensure we are stopped/clean/destroyed.
        mock().clear();
        cms::test::LibTeardownAll();
    }

    static void GiveProcessingTime()
    {
        //use our unit testing backdoor to service
        //the active object's internal queue. This avoids threading issues
        //with unit tests, creating 100% predictable unit tests.
        while (HLCS_ProcessOneEvent(EXECUTION_OPTION_UNIT_TEST)) {}
    }

    static void StartServiceToLocked()
    {
        mock(HW_LOCK_CTRL_MOCK).expectOneCall("Init");
        mock(HW_LOCK_CTRL_MOCK).expectOneCall("Lock");
        mock(CB_MOCK).expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(HLCS_LOCK_STATE_LOCKED));
        HLCS_Start(EXECUTION_OPTION_UNIT_TEST);
        GiveProcessingTime();
        mock().checkExpectations();
        CHECK_TRUE(HLCS_LOCK_STATE_LOCKED == HLCS_GetState());
    }

    static void TestUnlock()
    {
        mock(HW_LOCK_CTRL_MOCK).expectOneCall("Unlock");
        mock(CB_MOCK).expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(HLCS_LOCK_STATE_UNLOCKED));
        HLCS_RequestUnlockedAsync();
        GiveProcessingTime();
        mock().checkExpectations();
        CHECK_TRUE(HLCS_LOCK_STATE_UNLOCKED == HLCS_GetState());
    }

    static void StartServiceToUnlocked()
    {
        mock(HW_LOCK_CTRL_MOCK).expectOneCall("Init");
        mock(HW_LOCK_CTRL_MOCK).expectOneCall("Lock");
        mock(CB_MOCK).expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(HLCS_LOCK_STATE_LOCKED));
        HLCS_Start(EXECUTION_OPTION_UNIT_TEST);
        GiveProcessingTime();
        mock().checkExpectations();
        TestUnlock();
    }
};

TEST(HwLockCtrlServiceTests, given_startup_when_created_then_does_not_crash)
{
    //setup() is called by cpputest, which creates our unit under test.
}

TEST(HwLockCtrlServiceTests, given_startup_when_started_then_service_ensures_the_lock_is_locked)
{
    //when originally developed, this test contained all the code
    //in the below helper method. Since this "setup" was needed by
    //other tests, it was extracted into a helper method.
    //this pattern of developing tests, discovering the need for common
    //setup helper methods, takes place throughout coding of these unit tests.
    StartServiceToLocked();
}

TEST(HwLockCtrlServiceTests, given_locked_when_another_lock_request_then_service_is_silent)
{
    StartServiceToLocked();
    HLCS_RequestLockedAsync();
    GiveProcessingTime();
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests, given_locked_when_unlock_request_then_service_unlocks_the_driver_and_emits_status_callback)
{
    StartServiceToLocked();
    TestUnlock();
}

TEST(HwLockCtrlServiceTests, given_unlocked_when_another_unlock_request_then_service_is_silent)
{
    StartServiceToUnlocked();
    HLCS_RequestUnlockedAsync();
    GiveProcessingTime();
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests, given_locked_when_selftest_request_then_service_performs_selftest_emits_results_and_returns_to_locked)
{
    StartServiceToLocked();

    auto passed = HW_LOCK_CTRL_SELF_TEST_PASSED;
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("SelfTest").withOutputParameterReturning("outResult", &passed, sizeof(passed));
    mock(CB_MOCK).expectOneCall("SelfTestResultCallback").withIntParameter("result", static_cast<int>(HLCS_SELF_TEST_RESULT_PASS));
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("Lock");
    mock(CB_MOCK).expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(HLCS_LOCK_STATE_LOCKED));
    HLCS_RequestSelfTestAsync();
    GiveProcessingTime();
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests, given_unlocked_when_selftest_request_then_service_performs_selftest_emits_results_and_returns_to_unlocked)
{
    StartServiceToUnlocked();

    auto passed = HW_LOCK_CTRL_SELF_TEST_PASSED;
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("SelfTest").withOutputParameterReturning("outResult", &passed, sizeof(passed));
    mock(CB_MOCK).expectOneCall("SelfTestResultCallback").withIntParameter("result", static_cast<int>(HLCS_SELF_TEST_RESULT_PASS));
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("Unlock");
    mock(CB_MOCK).expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(HLCS_LOCK_STATE_UNLOCKED));
    HLCS_RequestSelfTestAsync();
    GiveProcessingTime();
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests,given_locked_when_selftest_request_which_fails_then_service_still_returns_to_locked)
{
    StartServiceToLocked();

    auto passed = HW_LOCK_CTRL_SELF_TEST_FAILED_POWER;
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("SelfTest").withOutputParameterReturning("outResult", &passed, sizeof(passed));
    mock(CB_MOCK).expectOneCall("SelfTestResultCallback").withIntParameter("result", static_cast<int>(HLCS_SELF_TEST_RESULT_FAIL));
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("Lock");
    mock(CB_MOCK).expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(HLCS_LOCK_STATE_LOCKED));
    HLCS_RequestSelfTestAsync();
    GiveProcessingTime();
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests, given_unlocked_when_selftest_request_which_fails_then_service_still_returns_to_unlocked)
{
    StartServiceToUnlocked();

    auto passed = HW_LOCK_CTRL_SELF_TEST_FAILED_MOTOR;
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("SelfTest").withOutputParameterReturning("outResult", &passed, sizeof(passed));
    mock(CB_MOCK).expectOneCall("SelfTestResultCallback").withIntParameter("result", static_cast<int>(HLCS_SELF_TEST_RESULT_FAIL));
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("Unlock");
    mock(CB_MOCK).expectOneCall("LockStateCallback").withIntParameter("state", static_cast<int>(HLCS_LOCK_STATE_UNLOCKED));
    HLCS_RequestSelfTestAsync();
    GiveProcessingTime();
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests, given_unlocked_when_5_secs_passes_then_checks_current) {
    StartServiceToUnlocked();
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("ReadCurrent").andReturnValue(100);
    cms::test::MoveTimeForward(5s);
    GiveProcessingTime();
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests, if_current_exceeds_2amps_will_assert) {
    StartServiceToUnlocked();
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("ReadCurrent").andReturnValue(2001);
    cms::test::AssertOutputDisable();
    cms::test::MockExpectAssert();
    cms::test::MoveTimeForward(5s);
    GiveProcessingTime();
    mock().checkExpectations();
}
