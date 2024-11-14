/// @brief Support methods to help with unit testing for FreeRTOS, memory allocation
///        related support, such as unique_ptr types for allocated queues, etc.
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
#ifndef CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_MEMORY_HPP
#define CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_MEMORY_HPP

#include "FreeRTOS.h"
#include "queue.h"
#include <memory>
#include <functional>

namespace cms {
    namespace test {

        struct FreeRTOSQueueDeleter
        {
            void operator()(struct QueueDefinition * handle)
            {
                if (handle != nullptr)
                {
                    vQueueDelete(handle);
                }
            }
        };

        using unique_queue = std::unique_ptr<struct QueueDefinition, FreeRTOSQueueDeleter>;
        using unique_sema = std::unique_ptr<struct QueueDefinition, FreeRTOSQueueDeleter>;

    } //namespace test
} //namespace cms

#endif //CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_MEMORY_HPP
