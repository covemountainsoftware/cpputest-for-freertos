/*
MIT License

Copyright (c) <2021> <Matthew Eshleman - https://covemountainsoftware.com>

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

/**
 * @brief the HwLockCtrlService (HLCS) provides for higher level hardware lock control behavior.
 *        For example, this service will automatically return the hardware lock to its last
 *        state after completing a self test request.
 *
 * @note: This service "is a" active object, and was created for demonstration purposes, to
 *        show the simplest active object pattern in a faux RTOS environment.
 *
 * @note: this file represents the public facing C API for this particular active object
 */

#ifndef HW_LOCK_CTRL_SERVICE_H
#define HW_LOCK_CTRL_SERVICE_H

#include <stdbool.h>
#include "cmsExecutionOption.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum HLCS_LockState
{
    HLCS_LOCK_STATE_UNKNOWN,
    HLCS_LOCK_STATE_LOCKED,
    HLCS_LOCK_STATE_UNLOCKED
} HLCS_LockStateT;

typedef enum HLCS_SelfTestResult
{
    HLCS_SELF_TEST_RESULT_PASS,
    HLCS_SELF_TEST_RESULT_FAIL
} HLCS_SelfTestResultT;

/**
 *  @brief HLCS_Init() will initialize the module and associated RTOS
 *         components. The module will be idle and not actually started.
 */
void HLCS_Init();

/**
 * @brief HLCS_Destroy() will stop (kill) the module and its internal thread
 *
 * @note: typical C module firmware designs may not include this type of
 *        public API. However, this is needed for clean unit testing and
 *        ultimately might prove useful in production code as well.
 */
void HLCS_Destroy();

/**
 * @brief HLCS_Start() will start behavior. Init() must have been called.
 */
void HLCS_Start(ExecutionOptionT option);

/**
 * @brief HLCS_GetState() provides a thread safe synchronous API to
 *            determine the current state of this module.
 * @return the current lock state
 */
HLCS_LockStateT HLCS_GetState();

typedef void (*HLCS_ChangeStateCallback)(HLCS_LockStateT state);
/**
 * @brief HLCS_RegisterChangeStateCallback() provides a method to enable
 *        a single external observer of this module's state.
 * @note: The callback will be executed in another thread context.
 *        The provided callback should be "fast" with minimal blocking.
 */
void HLCS_RegisterChangeStateCallback(HLCS_ChangeStateCallback callback);

typedef void (*HLCS_SelfTestResultCallback)(HLCS_SelfTestResultT result);
/**
 * @brief HLCS_RegisterSelfTestResultCallback() provides a method to enable
 *        a single external observer of this module's self test behavior.
 * @note: The callback will be executed in another thread context.
 *        The provided callback should be "fast" with minimal blocking.
 */
void HLCS_RegisterSelfTestResultCallback(HLCS_SelfTestResultCallback callback);

/**
 * @brief HLCS_RequestLockedAsync() issue an asynchronous request to this module
 *        to lock the hardware lock.
 */
void HLCS_RequestLockedAsync();

/**
 * @brief HLCS_RequestUnlockedAsync() issue an asynchronous request to this module
 *        to unlock the hardware lock.
 */
void HLCS_RequestUnlockedAsync();

/**
 * @brief HLCS_RequestUnlockedAsync() issue an asynchronous request to this module
 *        to perform a self test on the hardware lock.
 */
void HLCS_RequestSelfTestAsync();

/****************************************************************************/
/*****  Backdoor functionality provided for unit testing access only ********/
/****************************************************************************/

/**
 * @brief HLCS_ProcessOneEvent() - provided for unit testing
 *        access only.
 *
 * @param option EXECUTION_OPTION_NORMAL - internal, normal thread use
 *               EXECUTION_OPTION_UNIT_TEST - for unit testing
 *
 * @return true: a message was processed.
 *         false: internal queue was empty, nothing processed.
 */
bool HLCS_ProcessOneEvent(ExecutionOptionT option);

#ifdef __cplusplus
}
#endif

#endif //HW_LOCK_CTRL_SERVICE_H
