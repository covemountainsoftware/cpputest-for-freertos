
add_executable(demoPcApp main.cpp)
include_directories(${FREERTOS_KERNEL_PATH}/include)
target_link_libraries(demoPcApp  hwLockCtrlService freertos_kernel freertos_config)
