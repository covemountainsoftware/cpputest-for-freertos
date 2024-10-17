cmake_minimum_required(VERSION 3.16)
project(cpputest-for-freertos-lib VERSION 0.0.1)
include(FetchContent)

set(CMS_EXTERNALS_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals)
set(CMS_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cpputest-for-freertos-lib/cmake CACHE INTERNAL "")


add_library(freertos_config INTERFACE)
target_include_directories(freertos_config INTERFACE
        cpputest-for-freertos-lib/include)
set( FREERTOS_HEAP "3" CACHE STRING "" FORCE)
set( FREERTOS_PORT "GCC_POSIX" CACHE STRING "" FORCE)

if(NOT DEFINED CMS_FREERTOS_KERNEL_TOP_DIR)
    set(CMS_FREERTOS_KERNEL_TOP_DIR ${CMS_EXTERNALS_TOP_DIR}/FreeRTOS-Kernel)
    FetchContent_Declare(FreeRTOS-Kernel
            GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
            GIT_TAG        dbf70559b27d39c1fdb68dfb9a32140b6a6777a0 #v11.1.0
            SOURCE_DIR     ${CMS_FREERTOS_KERNEL_TOP_DIR}
    )
    message("Fetching FreeRTOS Kernel git repository")
    FetchContent_MakeAvailable(FreeRTOS-Kernel)
endif(NOT DEFINED CMS_FREERTOS_KERNEL_TOP_DIR)

FetchContent_Declare(fake-timers
        GIT_REPOSITORY https://github.com/covemountainsoftware/fake-timers.git
        GIT_TAG        0dd7edd4bfac80d8bede10e6ff7bc651df110ab4 #latest
        SOURCE_DIR     externals/fake-timers
)
message("Fetching fake-timers git repository")
FetchContent_MakeAvailable(fake-timers)



