
set(MOCKS_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/mocks CACHE INTERNAL "")
set(DRIVERS_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/drivers CACHE INTERNAL "")

include_directories(services/include)
add_subdirectory(services)
add_subdirectory(drivers)
add_subdirectory(apps)