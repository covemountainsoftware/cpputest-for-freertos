
set(TEST_APP_NAME cpputest-for-freertos-lib-tests)
set(TEST_SOURCES
        cpputest_freertos_assert_tests.cpp
        cpputest_freertos_timers_tests.cpp
        cpputest_freertos_queue_tests.cpp
)

# this include expects TEST_SOURCES and TEST_APP_NAME to be
# defined, and creates the cpputest based test executable target
include(${CMS_CMAKE_DIR}/cpputestCMake.cmake)

target_link_libraries(${TEST_APP_NAME} cpputest-for-freertos-lib  ${CPPUTEST_LDFLAGS})