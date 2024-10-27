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

#ifndef BUTTON_SERVICE_H
#define BUTTON_SERVICE_H

#include <stdbool.h>
#include "cmsExecutionOption.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Buttons supported on this product
 */
typedef enum ButtonFlag {
    BUTTON_ON_OFF = 0x1,
    BUTTON_FUNC_A = 0x2,
} ButtonFlagT;

typedef enum ButtonState {
    BUTTON_PRESSED,
    BUTTON_RELEASED,
} ButtonStateT;

/**
 * Initialize the Button Service. Does not start the thread, just prepares.
 */
void ButtonService_Init();

/**
 * Destroy the ButtonService. Typically only called in unit testing environment.
 */
void ButtonService_Destroy();

/**
 * Start the button service/thread. Init() must have been called first.
 * @param option
 */
void ButtonService_Start(ExecutionOptionT option);

/**
 * typedef for the button service callback
 */
typedef void (*ButtonService_ButtonChangeCallback)(ButtonFlagT flags, ButtonStateT state, void *context);

/**
 * Register callback to be executed from the button service
 * thread context. Guaranteed to NOT be called from an ISR context.
 * @param callback
 * @param context
 */
void ButtonService_RegisterButtonChangeCallback(ButtonService_ButtonChangeCallback callback, void *context);

/****************************************************************************/
/*****  Backdoor functionality provided for unit testing access only ********/
/****************************************************************************/

/**
 * provided for unit testing access only. For the button service,
 * will check if semaphore token is available or not (i.e.
 * the button ISR has occurred).
 *
 * @param option EXECUTION_OPTION_NORMAL - internal, normal thread use
 *               EXECUTION_OPTION_UNIT_TEST - for unit testing
 *
 * @return true: an event was processed
 *         false: no events, nothing processed.
 */
bool ButtonService_ProcessOneEvent(ExecutionOptionT option);

/**
 * Fake/simulate the ISR that the button service relies upon
 */
void ButtonService_SimulateIsr();

#ifdef __cplusplus
}
#endif

#endif //BUTTON_SERVICE_H
