
#ifndef CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_FAKE_QUEUE_HPP
#define CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_FAKE_QUEUE_HPP

#include <deque>
#include <vector>
#include "FreeRTOS.h"

typedef struct QueueDefinition
{
    UBaseType_t queueLength = {};
    UBaseType_t itemSize = {};
    uint8_t queueType = {};
    std::deque<std::vector<uint8_t>> queue = {};
    uint64_t recursiveCallCount = {};
    const char * registryName = nullptr;
    struct QueueDefinition * queueSetContainer = nullptr;
} FakeQueue;

namespace cms {
    BaseType_t InternalQueueReceive(FakeQueue *queue, void * const buffer);
    BaseType_t InternalQueueReceive(FakeQueue *queue);
} //namespace cms

#endif //CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_FAKE_QUEUE_HPP
