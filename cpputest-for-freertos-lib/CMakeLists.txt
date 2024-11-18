include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra -Werror)

set(CMS_EXTERNALS_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals)
set(CMS_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake CACHE INTERNAL "")

if(NOT DEFINED CMS_FREERTOS_KERNEL_TOP_DIR)
    set(CMS_FREERTOS_KERNEL_TOP_DIR ${CMS_EXTERNALS_TOP_DIR}/FreeRTOS-Kernel)
    FetchContent_Declare(FreeRTOS-Kernel
            GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
            GIT_TAG        dbf70559b27d39c1fdb68dfb9a32140b6a6777a0 #v11.1.0
            SOURCE_DIR     ${CMS_FREERTOS_KERNEL_TOP_DIR}
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
    )
    message("Fetching FreeRTOS Kernel git repository")
    FetchContent_GetProperties(FreeRTOS-Kernel)
    if(NOT FreeRTOS-Kernel_POPULATED)
        FetchContent_Populate(FreeRTOS-Kernel)
    endif()
endif(NOT DEFINED CMS_FREERTOS_KERNEL_TOP_DIR)

FetchContent_Declare(fake-timers
        GIT_REPOSITORY https://github.com/covemountainsoftware/fake-timers.git
        GIT_TAG        0dd7edd4bfac80d8bede10e6ff7bc651df110ab4 #latest
        SOURCE_DIR     ${CMS_EXTERNALS_TOP_DIR}/fake-timers
)
message("Fetching fake-timers git repository")
FetchContent_MakeAvailable(fake-timers)

include_directories(include)

set(FREERTOS_KERNEL_PATH ${CMS_FREERTOS_KERNEL_TOP_DIR} CACHE INTERNAL "")

add_library(cpputest-for-freertos-lib
        src/cpputest_for_freertos_task.cpp
        src/cpputest_for_freertos_queue.cpp
        src/cpputest_for_freertos_queue_set.cpp
        src/cpputest_for_freertos_assert.cpp
        src/cpputest_for_freertos_timers.cpp
        src/cpputest_main.cpp
        src/cpputest_for_freertos_semaphore.cpp
        src/cpputest_for_freertos_mutex.cpp
        include/cpputest_for_freertos_lib.hpp
)

add_subdirectory(tests)

target_include_directories(cpputest-for-freertos-lib PUBLIC  include port/include externals/FreeRTOS-Kernel/include)
target_link_libraries(cpputest-for-freertos-lib fake-timers-lib)
