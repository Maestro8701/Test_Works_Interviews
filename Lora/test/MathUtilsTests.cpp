//
// Created by never on 06.05.2025.
//
#include <unity.h>

#include "core/MathUtils.h"

/**
 * @brief Тестирует базовую работу метода экспоненциального сглаживания.
 *
 * Проверяется корректность вычислений при alpha = 0.5.
 * Ожидается результат: среднее значение между предыдущим и текущим.
 */
void test_exponential_smoothing_basic() {
    // Arrange
    constexpr float prev = 10.0f;
    constexpr float current = 20.0f;
    constexpr float alpha = 0.5f;

    // Act
    const float result = MathUtils::exponentialSmoothing(prev, current, alpha);

    // Assert
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 15.0f, result);  // (0.5 * 20 + 0.5 * 10)
}

/**
 * @brief Проверяет, что при alpha = 0 результат равен предыдущему значению.
 *
 * Это граничный случай, при котором новое значение не учитывается вовсе.
 */
void test_alpha_zero() {
    // Arrange
    constexpr float prev = 42.0f;
    constexpr float current = 100.0f;
    constexpr float alpha = 0.0f;

    // Act
    const float result = MathUtils::exponentialSmoothing(prev, current, alpha);

    // Assert
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, prev, result);
}

/**
 * @brief Проверяет, что при alpha = 1 результат равен новому значению.
 *
 * Это граничный случай, при котором игнорируется предыдущее значение.
 */
void test_alpha_one() {
    // Arrange
    constexpr float prev = 42.0f;
    constexpr float current = 100.0f;
    constexpr float alpha = 1.0f;

    // Act
    const float result = MathUtils::exponentialSmoothing(prev, current, alpha);

    // Assert
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, current, result);
}

/**
 * @brief Проверяет корректность обработки alpha < 0.
 *
 * Значение alpha должно быть автоматически ограничено нулём.
 * Ожидается, что результат совпадает с предыдущим значением.
 */
void test_alpha_below_zero_clamped() {
    // Arrange
    constexpr float prev = 30.0f;
    constexpr float current = 60.0f;
    constexpr float alpha = -0.5f;  // должно быть приведено к 0

    // Act
    const float result = MathUtils::exponentialSmoothing(prev, current, alpha);

    // Assert
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, prev, result);
}

/**
 * @brief Проверяет корректность обработки alpha > 1.
 *
 * Значение alpha должно быть автоматически ограничено единицей.
 * Ожидается, что результат совпадает с новым значением.
 */
void test_alpha_above_one_clamped() {
    // Arrange
    constexpr float prev = 30.0f;
    constexpr float current = 60.0f;
    constexpr float alpha = 1.5f;  // должно быть приведено к 1

    // Act
    const float result = MathUtils::exponentialSmoothing(prev, current, alpha);

    // Assert
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, current, result);
}

void setUp(void) {
}

void tearDown(void) {
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_exponential_smoothing_basic);
    RUN_TEST(test_alpha_zero);
    RUN_TEST(test_alpha_one);
    RUN_TEST(test_alpha_below_zero_clamped);
    RUN_TEST(test_alpha_above_one_clamped);
    return UNITY_END();
}
