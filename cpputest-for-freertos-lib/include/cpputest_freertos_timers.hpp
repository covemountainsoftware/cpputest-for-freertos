
#ifndef CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FREERTOS_TIMERS_HPP
#define CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FREERTOS_TIMERS_HPP

#include <chrono>

namespace cms {
namespace test {

    /**
     * Initialize the functional but fake CppUTest for FreeRTOS timers.
     */
    void TimersInit();

    /**
     * Clean up/destroy the fake CppUTest for FreeRTOS timers.
     */
    void TimersDestroy();

    /**
     * check if the current unit test has initialized the timer
     * subsystem or not.
     * @return true: yes, TimersInit() was called, so timers are active.
     */
    bool TimersIsActive();

    /**
     * Move Time Forward.
     * @param duration
     */
    void MoveTimeForward(std::chrono::nanoseconds duration);

    /**
     * Get the current time, as duration since Init was called.
     * @return
     */
    std::chrono::nanoseconds GetCurrentInternalTime();

} //namespace
}//namespace

#endif //CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FREERTOS_TIMERS_HPP
