/*
 *   This concrete implementation of the Button Reader "C" style interface
 *   is for demo purposes only. The focus of this project
 *   is how to test FreeRTOS threads, hence
 *   this fake driver module.
 */
#include "buttonReader.h"
#include <stdio.h>

static ButtonReaderIsrCallback s_isrCallback;
static void* s_isrContext;

bool ButtonReaderInit()
{
    printf("%s() executed\n", __FUNCTION__);
    s_isrCallback = NULL;
    s_isrContext = NULL;
    return true;
}

bool ButtonReaderRead(uint16_t* pressed, uint16_t* released)
{
    static bool toggle = false;
    printf("%s() executed\n", __FUNCTION__);

    if ((pressed == NULL) || (released == NULL))
    {
        return false;
    }

    if (toggle)
    {
        *pressed = 2;
        *released = 0;
    }
    else
    {
        *pressed = 0;
        *released = 2;
    }

    toggle = !toggle;

    return true;
}

bool ButtonReaderRegisterIsrCallback(ButtonReaderIsrCallback callback, void* context)
{
    printf("%s() executed\n", __FUNCTION__);

    s_isrCallback = callback;
    s_isrContext = context;
    return true;
}
