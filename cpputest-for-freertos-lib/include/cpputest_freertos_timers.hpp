
#ifndef CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FREERTOS_TIMERS_HPP
#define CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FREERTOS_TIMERS_HPP

#include <chrono>

namespace cms {
namespace test {

    void InitTimers();
    void DestroyTimers();
    void MoveTimeForward(std::chrono::nanoseconds duration);

} //namespace
}//namespace

#endif //CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FREERTOS_TIMERS_HPP
