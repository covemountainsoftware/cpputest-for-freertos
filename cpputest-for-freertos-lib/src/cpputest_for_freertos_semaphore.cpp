#include "cpputest_for_freertos_fake_queue.hpp"
#include <cstring>
#include "queue.h"
#include "semphr.h"

extern "C" BaseType_t xQueueSemaphoreTake(QueueHandle_t queue, TickType_t ticks)
{
    (void) ticks;  //ignore in cpputest fake semaphore

    uint64_t dummy;
    return xQueueReceive(queue, &dummy, ticks);
}


extern "C" BaseType_t xQueueGiveFromISR(QueueHandle_t queue,
                                        BaseType_t * const pxHigherPriorityTaskWoken)
{
    (void)pxHigherPriorityTaskWoken;
    return xSemaphoreGive(queue);
}

extern "C" QueueHandle_t xQueueCreateCountingSemaphore(const UBaseType_t maxCount,
                                                       const UBaseType_t initialCount)
{
    QueueHandle_t sema = xQueueCreate(maxCount, 0);
    if (sema == nullptr)
    {
        return nullptr;
    }

    for (int i = 0; i < initialCount; ++i)
    {
        xSemaphoreGive(sema);
    }

    return sema;
}