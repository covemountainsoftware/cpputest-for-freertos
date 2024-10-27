/*
MIT License

Copyright (c) <2024> <Matthew Eshleman - https://covemountainsoftware.com>

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

#include "buttonService.h"
#include "cpputest_freertos_task.hpp"
#include "mockButtonReader.hpp"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

//a bit of internal knowledge the test needs to know about the hardware design
static const uint16_t ON_OFF_BIT = 0x02;
static const uint16_t FUNC_A_BIT = 0x08;

TEST_GROUP(HwLockCtrlServiceTests) {

    uint8_t testContextObject = 1;

    void setup() final {
        cms::test::TaskInit();
        mock("ButtonReader").expectOneCall("Init");
        ButtonService_Init();
        ButtonService_Start(EXECUTION_OPTION_UNIT_TEST);

        ButtonService_RegisterButtonChangeCallback([](ButtonFlagT flags, ButtonStateT state, void *context){
            mock("TEST").actualCall("Callback")
                    .withParameter("flags", flags)
                    .withParameter("state", state)
                    .withParameter("context", context);
        }, &testContextObject);
        mock().checkExpectations();
        mock().clear();
    }

    void teardown() final {
        ButtonService_Destroy(); //ensure we are stopped/clean/destroyed.
        mock().clear();
        cms::test::TaskDestroy();
    }

    static void GiveProcessingTime() {
        //use our unit testing backdoor to service
        //the active object's internal queue. This avoids threading issues
        //with unit tests, creating 100% predictable unit tests.
        while (ButtonService_ProcessOneEvent(EXECUTION_OPTION_UNIT_TEST)) {}
    }

    static void DoButtonIsr(uint16_t pressedBits, uint16_t releasedBits)
    {
        mock("ButtonReader")
           .expectOneCall("Read")
           .withOutputParameterReturning("pressed", &pressedBits, sizeof(pressedBits))
           .withOutputParameterReturning("released", &releasedBits, sizeof(releasedBits));
        cms::test::mock::ButtonReaderDoIsr();
        GiveProcessingTime();
    }
};

TEST(HwLockCtrlServiceTests, given_startup_when_created_then_does_not_crash)
{
    //setup() is called by cpputest, which inits AND starts our unit under test.
}

TEST(HwLockCtrlServiceTests, given_button_isr_then_reads_button_status)
{
    mock("ButtonReader").expectOneCall("Read").ignoreOtherParameters();
    cms::test::mock::ButtonReaderDoIsr();
    GiveProcessingTime();
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests, given_button_isr_and_registered_callback_then_reads_button_status_and_executes_service_callback)
{
    mock("TEST").expectOneCall("Callback")
       .withParameter("flags", BUTTON_ON_OFF)
        .withParameter("state", BUTTON_PRESSED)
        .ignoreOtherParameters();

    DoButtonIsr(ON_OFF_BIT, 0);

    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests, given_button_isr_and_registered_callback_then_callback_provides_correct_context)
{
    mock("TEST").expectOneCall("Callback")
            .withParameter("context", (void*)&testContextObject)
            .ignoreOtherParameters();

    DoButtonIsr(ON_OFF_BIT, 0);

    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests, given_button_released_and_then_callback_is_released_only)
{
    mock("TEST").expectOneCall("Callback")
            .withParameter("state", BUTTON_RELEASED)
            .ignoreOtherParameters();

    DoButtonIsr(0, ON_OFF_BIT);
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests, given_button_released_and_pressed_then_two_callbacks)
{
    mock("TEST").expectOneCall("Callback")
            .withParameter("state", BUTTON_PRESSED)
            .withParameter("flags", BUTTON_FUNC_A)
            .ignoreOtherParameters();

    mock("TEST").expectOneCall("Callback")
            .withParameter("state", BUTTON_RELEASED)
            .withParameter("flags", BUTTON_ON_OFF)
            .ignoreOtherParameters();

    DoButtonIsr(FUNC_A_BIT, ON_OFF_BIT);
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests, given_garbage_button_read_then_callback_is_not_executed)
{
    mock("TEST").expectNoCall("Callback");
    DoButtonIsr(1, 1);
    mock().checkExpectations();
}