/*
MIT License

Copyright (c) <2024> <Matthew Eshleman - https://covemountainsoftware.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/**
 * @brief public header for the Button Reader driver.
 *        This driver provides methods to read
 *        the status of up to 16 buttons.
 *        Additionally, it provides an ISR callback.
 *
  */
#ifndef BUTTON_READER_H
#define BUTTON_READER_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  initializes the driver.
 * @return true - initialization completed successfully.
 *         false - some error.
 */
bool ButtonReaderInit();

/**
 * Read the pressed/released status. Guaranteed atomic.
 * @param pressed: which buttons pressed since last read.
 * @param released: which buttons released since last read.
 * @return true: read ok. otherwise error.
 */
bool ButtonReaderRead(uint16_t* pressed, uint16_t* released);

/*
 * ISR Function Callback type.
 */
typedef void (*ButtonReaderIsrCallback)(void* context);

/**
 * Register a callback to be executed when the button change ISR is hit.
 * @param callback
 * @param context
 * @return
 */
bool ButtonReaderRegisterIsrCallback(ButtonReaderIsrCallback callback, void* context);

#ifdef __cplusplus
}
#endif

#endif //BUTTON_READER_H
