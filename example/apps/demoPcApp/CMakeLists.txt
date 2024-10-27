
# setup the FreeRTOS config interface, i.e. where the build will find the FreeRTOSConfig.h
# header file.
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
set(FREERTOS_HEAP "3" CACHE STRING "" FORCE)
set(FREERTOS_PORT "GCC_POSIX" CACHE STRING "" FORCE)

FetchContent_Declare(FreeRTOS-Kernel-DemoPcApp
        GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
        GIT_TAG        dbf70559b27d39c1fdb68dfb9a32140b6a6777a0 #v11.1.0
        SOURCE_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/externals/FreeRTOS-Kernel
)
message("Demo App: Fetching FreeRTOS Kernel git repository")
FetchContent_MakeAvailable(FreeRTOS-Kernel-DemoPcApp)

add_executable(demoPcApp main.cpp)
include_directories(${FREERTOS_KERNEL_PATH}/include)
target_link_libraries(demoPcApp hwLockCtrlService buttonService freertos_kernel freertos_config)
