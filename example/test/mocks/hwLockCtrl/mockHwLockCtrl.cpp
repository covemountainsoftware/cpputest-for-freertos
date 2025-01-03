/*
MIT License

Copyright (c) <2019-2020> <Matthew Eshleman - https://covemountainsoftware.com>

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

#include "hwLockCtrl.h"
#include "CppUTestExt/MockSupport.h"

static constexpr const char* MOCK_NAME = "HwLockCtrl";

bool HwLockCtrlInit()
{
    mock(MOCK_NAME).actualCall("Init");
    return static_cast<bool>(mock(MOCK_NAME).returnIntValueOrDefault(true)); //use IntValue due to bug in CppUTest bool handling.
}

bool HwLockCtrlLock()
{
    mock(MOCK_NAME).actualCall("Lock");
    return static_cast<bool>(mock(MOCK_NAME).returnIntValueOrDefault(true));
}

bool HwLockCtrlUnlock()
{
    mock(MOCK_NAME).actualCall("Unlock");
    return static_cast<bool>(mock(MOCK_NAME).returnIntValueOrDefault(true));
}

bool HwLockCtrlSelfTest(HwLockCtrlSelfTestResultT* outResult)
{
    mock(MOCK_NAME).actualCall("SelfTest").withOutputParameter("outResult", outResult);
    return static_cast<bool>(mock(MOCK_NAME).returnIntValueOrDefault(true));
}

int32_t HwLockCtrlReadCurrent()
{
    mock(MOCK_NAME).actualCall("ReadCurrent");
    return mock(MOCK_NAME).returnIntValueOrDefault(50);
}
