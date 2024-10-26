#include "FreeRTOS.h"
#include "semphr.h"
#include "CppUTest/TestHarness.h"

TEST_GROUP(SemaphoreTests)
{
    SemaphoreHandle_t mSemaUnderTest;

    void setup() final
    {
    }

    void teardown() final
    {
        if (mSemaUnderTest != nullptr)
        {
            vSemaphoreDelete(mSemaUnderTest);
        }
    }

    void CreateBinarySemaphore()
    {
        mSemaUnderTest = xSemaphoreCreateBinary();
        CHECK_TRUE(mSemaUnderTest != nullptr);
    }

    void CreateCountingSemaphore(uint32_t count = 10, uint32_t initial = 0)
    {
        mSemaUnderTest = xSemaphoreCreateCounting(count, initial);
        CHECK_TRUE(mSemaUnderTest != nullptr);
    }
};

TEST(SemaphoreTests, can_create_a_binary_semaphore)
{
    CreateBinarySemaphore();
}

TEST(SemaphoreTests, can_read_binary_semaphore_count)
{
    CreateBinarySemaphore();

    auto count = uxSemaphoreGetCount(mSemaUnderTest);
    CHECK_EQUAL(0, count);
}

TEST(SemaphoreTests, can_read_binary_semaphore_count_after_give)
{
    CreateBinarySemaphore();
    CHECK_EQUAL(0, uxSemaphoreGetCount(mSemaUnderTest));

    xSemaphoreGive(mSemaUnderTest);
    CHECK_EQUAL(1, uxSemaphoreGetCount(mSemaUnderTest));
}

TEST(SemaphoreTests, binary_semaphore_count_is_one_after_multiple_gives)
{
    CreateBinarySemaphore();
    CHECK_EQUAL(0, uxSemaphoreGetCount(mSemaUnderTest));
    CHECK_EQUAL(pdTRUE, xSemaphoreGive(mSemaUnderTest));
    CHECK_EQUAL(pdFALSE, xSemaphoreGive(mSemaUnderTest));
    CHECK_EQUAL(pdFALSE, xSemaphoreGive(mSemaUnderTest));
    CHECK_EQUAL(1, uxSemaphoreGetCount(mSemaUnderTest));
}

TEST(SemaphoreTests, binary_semaphore_count_is_zero_after_give_then_take)
{
    CreateBinarySemaphore();
    CHECK_EQUAL(0, uxSemaphoreGetCount(mSemaUnderTest));

    xSemaphoreGive(mSemaUnderTest);
    CHECK_EQUAL(1, uxSemaphoreGetCount(mSemaUnderTest));

    CHECK_EQUAL(pdTRUE, xSemaphoreTake(mSemaUnderTest, 1000));
    CHECK_EQUAL(0, uxSemaphoreGetCount(mSemaUnderTest));
}

TEST(SemaphoreTests, binary_semaphore_take_rtn_false_when_not_yet_signaled)
{
    CreateBinarySemaphore();
    CHECK_EQUAL(0, uxSemaphoreGetCount(mSemaUnderTest));
    CHECK_EQUAL(pdFALSE, xSemaphoreTake(mSemaUnderTest, 1000));
}

TEST(SemaphoreTests, can_create_a_counting_semaphore)
{
    CreateCountingSemaphore();
}

TEST(SemaphoreTests, counting_semaphore_honors_initial_count)
{
    CreateCountingSemaphore(10, 3);
    CHECK_EQUAL(3, uxSemaphoreGetCount(mSemaUnderTest));
}

TEST(SemaphoreTests, counting_semaphore_honors_initial_count_when_zero)
{
    CreateCountingSemaphore(10, 0);
    CHECK_EQUAL(0, uxSemaphoreGetCount(mSemaUnderTest));
}

TEST(SemaphoreTests, counting_semaphore_rtns_false_when_exceed_give_max)
{
    CreateCountingSemaphore(3, 0);
    CHECK_EQUAL(pdTRUE, xSemaphoreGive(mSemaUnderTest));
    CHECK_EQUAL(pdTRUE, xSemaphoreGive(mSemaUnderTest));
    CHECK_EQUAL(pdTRUE, xSemaphoreGive(mSemaUnderTest));
    CHECK_EQUAL(pdFALSE, xSemaphoreGive(mSemaUnderTest));
}

TEST(SemaphoreTests, counting_semaphore_give_take_match)
{
    CreateCountingSemaphore(3, 0);
    CHECK_EQUAL(pdTRUE, xSemaphoreGive(mSemaUnderTest));
    CHECK_EQUAL(pdTRUE, xSemaphoreGive(mSemaUnderTest));
    CHECK_EQUAL(pdTRUE, xSemaphoreGive(mSemaUnderTest));
    CHECK_EQUAL(pdTRUE, xSemaphoreTake(mSemaUnderTest, 1000));
    CHECK_EQUAL(pdTRUE, xSemaphoreTake(mSemaUnderTest, 1000));
    CHECK_EQUAL(pdTRUE, xSemaphoreTake(mSemaUnderTest, 1000));
    CHECK_EQUAL(pdFALSE, xSemaphoreTake(mSemaUnderTest, 1000));
}
