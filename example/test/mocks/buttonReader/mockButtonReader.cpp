#include "buttonReader.h"
#include "mockButtonReader.hpp"
#include "CppUTestExt/MockSupport.h"

static constexpr const char* MOCK_NAME = "ButtonReader";
static ButtonReaderIsrCallback s_lastCallback = nullptr;
static void* s_lastContext = nullptr;

bool ButtonReaderInit()
{
    s_lastCallback = nullptr;
    s_lastContext = nullptr;
    mock(MOCK_NAME).actualCall("Init");
    return mock(MOCK_NAME).returnBoolValueOrDefault(true);
}

bool ButtonReaderRead(uint16_t* pressed, uint16_t* released)
{
    mock(MOCK_NAME)
      .actualCall("Read")
      .withOutputParameter("pressed", pressed)
      .withOutputParameter("released", released);
    return mock(MOCK_NAME).returnBoolValueOrDefault(true);
}

bool ButtonReaderRegisterIsrCallback(ButtonReaderIsrCallback callback, void* context)
{
    //skip the mock, make this behavior a 'fake' instead
    s_lastCallback = callback;
    s_lastContext = context;
    return true;
}

namespace cms::test::mock {

    void ButtonReaderDoIsr()
    {
        if (s_lastCallback == nullptr)
        {
            return;
        }

        s_lastCallback(s_lastContext);
    }
}


