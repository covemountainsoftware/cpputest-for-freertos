/// @brief Provides an implementation of fake but functional
///        FreeRTOS software timers.
///
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
#include "FakeTimers.hpp"

namespace cms {
namespace test {

    static FakeTimers* s_fakeTimers = nullptr;

    void TimersInit()
    {
        configASSERT(s_fakeTimers == nullptr);

        std::chrono::milliseconds sysTick { configTICK_RATE_HZ * 1/1000 };
        s_fakeTimers = new FakeTimers(sysTick);
    }

    void TimersDestroy()
    {
        configASSERT(s_fakeTimers != nullptr);
        delete s_fakeTimers;
        s_fakeTimers = nullptr;
    }

    bool TimersIsActive()
    {
        return s_fakeTimers != nullptr;
    }

    void MoveTimeForward(std::chrono::nanoseconds duration)
    {
        configASSERT(s_fakeTimers != nullptr);
        s_fakeTimers->MoveTimeForward(duration);
    }

    std::chrono::nanoseconds GetCurrentInternalTime()
    {
        if (TimersIsActive())
        {
            return s_fakeTimers->GetCurrentInternalTime();
        }
        else
        {
            return std::chrono::nanoseconds (0);
        }
    }

    std::chrono::nanoseconds TicksToChrono(TickType_t ticks)
    {
        std::chrono::milliseconds rtn(pdTICKS_TO_MS(ticks));
        return rtn;
    }

    static const uint8_t s_an_object = 1;

    void* HandleToPointer(FakeTimers::Handle handle)
    {
        return (void*)((&s_an_object) + handle);
    }

    FakeTimers::Handle PointerToHandle(void* ptr)
    {
        return ((uint8_t*)ptr - &s_an_object);
    }

} //namespace test
} //namespace cms

using namespace cms::test;

extern "C" TimerHandle_t xTimerCreate( const char * const pcTimerName,
                            const TickType_t xTimerPeriodInTicks,
                            const BaseType_t xAutoReload,
                            void * const pvTimerID,
                            TimerCallbackFunction_t pxCallbackFunction )
{
    configASSERT(s_fakeTimers != nullptr);
    auto behavior = FakeTimers::Behavior::SingleShot;
    if (xAutoReload == pdTRUE)
    {
        behavior = FakeTimers::Behavior::AutoReload;
    }
    auto handle = s_fakeTimers->TimerCreate(pcTimerName,
                               TicksToChrono(xTimerPeriodInTicks),
                               behavior, pvTimerID,
                               [=](FakeTimers::Handle handle, FakeTimers::Context){
                                                pxCallbackFunction(
                                                        (TimerHandle_t)cms::test::HandleToPointer(handle));
                                });

    return (TimerHandle_t)HandleToPointer(handle);
}

extern "C" BaseType_t xTimerGenericCommandFromTask( TimerHandle_t xTimer,
                                         const BaseType_t xCommandID,
                                         const TickType_t xOptionalValue,
                                         BaseType_t * const pxHigherPriorityTaskWoken,
                                         const TickType_t xTicksToWait )
{
    configASSERT(s_fakeTimers != nullptr);
    configASSERT(xTimer != nullptr);

    (void)pxHigherPriorityTaskWoken;
    (void)xTicksToWait;

    switch (xCommandID) {
        case tmrCOMMAND_START: {
            bool ok = s_fakeTimers->TimerStart(PointerToHandle(xTimer));
            configASSERT(ok);
            break;
        }
        case tmrCOMMAND_DELETE: {
            bool ok = s_fakeTimers->TimerDelete(PointerToHandle(xTimer));
            configASSERT(ok);
            break;
        }
        case tmrCOMMAND_STOP: {
            bool ok = s_fakeTimers->TimerStop(PointerToHandle(xTimer));
            configASSERT(ok);
            break;
        }
        case tmrCOMMAND_CHANGE_PERIOD: {
            bool ok = s_fakeTimers->TimerChangePeriod(
                    PointerToHandle(xTimer),
                    std::chrono::milliseconds (pdTICKS_TO_MS(xOptionalValue)));
            configASSERT(ok);
            break;
        }
        case tmrCOMMAND_RESET: {
            bool ok = s_fakeTimers->TimerReset(PointerToHandle(xTimer));
            configASSERT(ok);
            break;
        }
        default:
            configASSERT(true == false);
            break;
    }

    return pdPASS;
}

extern "C" BaseType_t xTimerIsTimerActive( TimerHandle_t xTimer )
{
    configASSERT(s_fakeTimers != nullptr);
    bool active = s_fakeTimers->TimerIsActive(PointerToHandle(xTimer));
    if (active)
    {
        return pdTRUE;
    }
    else
    {
        return pdFALSE;
    }
}

extern "C" void * pvTimerGetTimerID( const TimerHandle_t xTimer )
{
    configASSERT(s_fakeTimers != nullptr);
    auto context = s_fakeTimers->TimerGetContext(PointerToHandle(xTimer));
    return context;
}

extern "C" void vTimerSetTimerID( TimerHandle_t xTimer, void * pvNewID )
{
    configASSERT(s_fakeTimers != nullptr);
    s_fakeTimers->TimerSetContext(PointerToHandle(xTimer), pvNewID);
}

extern "C" const char * pcTimerGetName( TimerHandle_t xTimer )
{
    configASSERT(s_fakeTimers != nullptr);
    return s_fakeTimers->TimerGetName(PointerToHandle(xTimer));
}

extern "C" BaseType_t xTimerGetReloadMode(TimerHandle_t xTimer)
{
    configASSERT(s_fakeTimers != nullptr);
    auto behavior = s_fakeTimers->TimerGetBehavior(PointerToHandle(xTimer));
    if (behavior == cms::test::FakeTimers::Behavior::AutoReload)
    {
        return pdTRUE;
    }
    else
    {
        return pdFALSE;
    }
}

extern "C" void vTimerSetReloadMode(TimerHandle_t xTimer, const BaseType_t xAutoReload)
{
    configASSERT(s_fakeTimers != nullptr);

    if (xAutoReload == pdTRUE)
    {
        s_fakeTimers->TimerSetBehavior(PointerToHandle(xTimer), FakeTimers::Behavior::AutoReload);
    }
    else
    {
        s_fakeTimers->TimerSetBehavior(PointerToHandle(xTimer), FakeTimers::Behavior::SingleShot);
    }
}

extern "C" TickType_t xTimerGetPeriod( TimerHandle_t xTimer )
{
    configASSERT(s_fakeTimers != nullptr);
    auto period = s_fakeTimers->TimerGetPeriod(PointerToHandle(xTimer));
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(period);
    return pdMS_TO_TICKS(milliseconds.count());
}

extern "C" TickType_t xTimerGetExpiryTime( TimerHandle_t xTimer )
{
    configASSERT(s_fakeTimers != nullptr);
    auto period = s_fakeTimers->TimerGetExpiryTime(PointerToHandle(xTimer));
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(period);
    return pdMS_TO_TICKS(milliseconds.count());
}