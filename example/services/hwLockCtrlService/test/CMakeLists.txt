#set(TEST_APP_NAME HwLockCtrlServiceTests)

#note: we are building and linking with the MOCK LockCtrl module, instead
#      of the actual LockCtrl driver.
#set(TEST_SOURCES hwLockCtrlServiceTests.cpp
#        ../../../test/common/cpputestMain.cpp
#        ../src/hwLockCtrlService.c
#        ../../../test/mocks/hwLockCtrl/mockHwLockCtrl.cpp)
#
#include(../../../test/common/cpputestCMake.txt)
#include_directories(../../../drivers/hwLockCtrl/include)
#
#target_link_libraries(${TEST_APP_NAME} Threads::Threads fauxRTOS)