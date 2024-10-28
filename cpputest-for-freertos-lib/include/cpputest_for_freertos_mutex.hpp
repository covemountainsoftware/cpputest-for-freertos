
#ifndef CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_MUTEX_HPP
#define CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_MUTEX_HPP

namespace cms {
namespace test {

    /**
     * Initialize the mutex state tracking,
     * such that this unit test, when Teardown is called,
     * will confirm that all mutexes are unlocked.
     */
    void MutexTrackingInit();

    /**
     * Check the state of any active mutexes.
     * If a mutex is found to be locked, then that is considered
     * a test failure. i.e. confirm that all mutex usage during
     * a unit test locks and then unlocks.
     */
    void MutexTrackingTeardown();

    /**
     * Check if any active mutexes are in a locked
     * state.
     * @return
     */
    bool IsAnyMutexLocked();

} //namespace
}//namespace

#endif //CPPUTEST_FOR_FREERTOS_LIB_CPPUTEST_FOR_FREERTOS_MUTEX_HPP
