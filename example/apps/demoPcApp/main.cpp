#include <cstdio>
#include <iostream>
#include <string>
#include <cassert>
#include "FreeRTOS.h"
#include "task.h"
#include "hwLockCtrlService.h"
#include "buttonService.h"

static HLCS_LockStateT s_lastState = HLCS_LOCK_STATE_UNKNOWN;

std::string GetLockState()
{
    switch (s_lastState)
    {
    case HLCS_LOCK_STATE_UNLOCKED:
        return "Unlocked";
    case HLCS_LOCK_STATE_LOCKED:
        return "Locked";
    default:
        return "Unknown";
    }
}

void LockStateChangeCallback(HLCS_LockStateT state)
{
    //NOTE: this callback is being executed
    //      in the thread context of the HwLockCtrlService.
    s_lastState = state;
    std::cout << "Lock state changed: " << GetLockState() << std::endl;
}

void SelfTestResultCallback(HLCS_SelfTestResultT result)
{
    //NOTE: this callback is being executed
    //      in the thread context of the HwLockCtrlService.

    const char* resultStr = (result == HLCS_SELF_TEST_RESULT_PASS) ? "Pass" : "Fail";
    std::cout << "Self Test result: " << resultStr << std::endl;
}

enum class DesiredAction
{
    EXIT,
    LOCK,
    UNLOCK,
    SELF_TEST
};

DesiredAction GetUserAction()
{
    std::string action;

    std::cout << "Lock state is: " << GetLockState() << std::endl;
    std::cout << "0: Unlock the lock" << std::endl;
    std::cout << "1: Lock the lock" << std::endl;
    std::cout << "2: Self Test the lock" << std::endl;
    std::cout << "3: Button ISR" << std::endl;
    std::cout << "9: Exit" << std::endl
              << std::endl;

    while (true) {
        int ch = getchar();
        switch (ch) {
            case '0':
                return DesiredAction::UNLOCK;
            case '1':
                return DesiredAction::LOCK;
            case '2':
                return DesiredAction::SELF_TEST;
            case '3':
                ButtonService_SimulateIsr();
                continue;
            case '9':
                return DesiredAction::EXIT;
            default:
                vTaskDelay(10);
                continue;
        }
    }
}

void UserInputTask(void*)
{
    while (true)
    {
        switch (GetUserAction())
        {
            case DesiredAction::LOCK:
                HLCS_RequestLockedAsync();
                break;
            case DesiredAction::UNLOCK:
                HLCS_RequestUnlockedAsync();
                break;
            case DesiredAction::SELF_TEST:
                HLCS_RequestSelfTestAsync();
                break;
            default:
                HLCS_Destroy();
                exit(0);
        }
    }
}

void ButtonChangeCallback(ButtonFlagT flags, ButtonStateT state, void* context)
{
    (void)context;
    printf("%s(flags = 0x%x, state = %d)\n",
           __FUNCTION__ , flags, state);
}

int main()
{
    HLCS_Init();
    HLCS_RegisterChangeStateCallback(LockStateChangeCallback);
    HLCS_RegisterSelfTestResultCallback(SelfTestResultCallback);
    HLCS_Start(EXECUTION_OPTION_NORMAL);

    ButtonService_Init();
    ButtonService_RegisterButtonChangeCallback(ButtonChangeCallback, nullptr);
    ButtonService_Start(EXECUTION_OPTION_NORMAL);

    TaskHandle_t input_thread = nullptr;
    BaseType_t ok = xTaskCreate(UserInputTask, "Input", 2000,
                                nullptr, tskIDLE_PRIORITY+2, &input_thread);
    assert(ok == pdPASS);

    //kick off FreeRTOS
    vTaskStartScheduler();
}

void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    char * pcTaskName )
{
    /* Check pcTaskName for the name of the offending task,
     * or pxCurrentTCB if pcTaskName has itself been corrupted. */
    ( void ) xTask;
    fprintf(stderr, "!! --> Overflow: %s\n", pcTaskName);
}
