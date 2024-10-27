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

#include <stdatomic.h>
#include <stdio.h>
#include "buttonService.h"
#include "buttonReader.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h" //freertos semaphores

static void ButtonService_Task(void*);
static void ButtonService_Initialize();
static void ButtonService_IsrCallback(void* context);
static void Emit(uint16_t bits, ButtonStateT state);

static TaskHandle_t s_thread = NULL;
static SemaphoreHandle_t s_eventSema = NULL;
static atomic_bool s_exitThread = false;
static ButtonService_ButtonChangeCallback s_callback = NULL;
static void* s_context = NULL;

static const uint16_t ON_OFF_BIT = 0x02;
static const uint16_t FUNC_A_BIT = 0x08;

void ButtonService_Init()
{
    configASSERT(s_thread == NULL);
    configASSERT(s_eventSema == NULL);
    configASSERT(s_callback == NULL);
    configASSERT(s_context == NULL);
    configASSERT(s_exitThread == false);

    s_eventSema = xSemaphoreCreateBinary();
    configASSERT(s_eventSema != 0);

    //thread is created in Start()
}

void ButtonService_Destroy()
{
    if (s_eventSema != NULL)
    {
        s_exitThread = true;
        xSemaphoreGive(s_eventSema);
        vTaskDelete(s_thread);
        vSemaphoreDelete(s_eventSema);
    }

    s_exitThread = false;
    s_eventSema = NULL;
    s_thread = NULL;
    s_callback = NULL;
    s_context = NULL;
}

void ButtonService_Start(ExecutionOptionT option)
{
    configASSERT(s_thread == NULL);
    if (EXECUTION_OPTION_NORMAL == option)
    {
        BaseType_t ok = xTaskCreate(ButtonService_Task, "ButtonService", 2000, NULL, tskIDLE_PRIORITY+3, &s_thread);
        configASSERT(ok == pdPASS);
    }
    else
    {
        ButtonService_Initialize();
    }
}

void ButtonService_RegisterButtonChangeCallback(ButtonService_ButtonChangeCallback callback, void* context)
{
    s_callback = callback;
    s_context = context;
}

bool ButtonService_ProcessOneEvent(ExecutionOptionT option)
{
    if ((EXECUTION_OPTION_UNIT_TEST == option) &&
        (0 == uxSemaphoreGetCount(s_eventSema)))
    {
        return false;
    }

    BaseType_t rtn = xSemaphoreTake(s_eventSema, portMAX_DELAY);
    if (rtn == pdFALSE)
    {
        return false;
    }

    uint16_t pressed = 0;
    uint16_t released = 0;
    bool ok = ButtonReaderRead(&pressed, &released);
    configASSERT(ok);

    if (s_callback != NULL)
    {
        if (pressed != 0)
        {
            Emit(pressed, BUTTON_PRESSED);
        }

        if (released != 0)
        {
            Emit(released, BUTTON_RELEASED);
        }
    }

    return true;
}

void ButtonService_Initialize()
{
    ButtonReaderInit();
    ButtonReaderRegisterIsrCallback(ButtonService_IsrCallback, NULL);
}

void ButtonService_Task(void* params)
{
    (void) params;

    ButtonService_Initialize();
    while (!s_exitThread)
    {
        ButtonService_ProcessOneEvent(EXECUTION_OPTION_NORMAL);
    }
}

void ButtonService_IsrCallback(void* context)
{
    (void)context;
    BaseType_t higherPriority = pdFALSE;
    xSemaphoreGiveFromISR(s_eventSema, &higherPriority);
    portYIELD_FROM_ISR(higherPriority);
}

void ButtonService_SimulateIsr()
{
    ButtonService_IsrCallback(NULL);
}

void Emit(uint16_t bits, ButtonStateT state)
{
    ButtonFlagT flags = 0;
    if (bits & ON_OFF_BIT)
    {
        flags |= BUTTON_ON_OFF;
    }

    if (bits & FUNC_A_BIT)
    {
        flags |= BUTTON_FUNC_A;
    }

    if ((s_callback != NULL) && (flags != 0))
    {
        s_callback(flags, state, s_context);
    }
}

