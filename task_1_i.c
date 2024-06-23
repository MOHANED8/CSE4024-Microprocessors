#include "stm32f4xx.h"  // Ensure correct header file for your development environment
#include <stdbool.h>
#define LED1_PIN GPIO_PIN_5
#define LED2_PIN GPIO_PIN_6
#define LED3_PIN GPIO_PIN_7
#define BUTTON_PIN GPIO_PIN_13

void setup() {
    // Initialize GPIO for LEDs
    __HAL_RCC_GPIOA_CLK_ENABLE();  // Enable clock for GPIOA
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = LED1_PIN | LED2_PIN | LED3_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Initialize GPIO for Button
    __HAL_RCC_GPIOC_CLK_ENABLE();  // Enable clock for GPIOC
    GPIO_InitStruct.Pin = BUTTON_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void loop() {
    static uint8_t counter = 0;  // Keeps count value between function calls
    static bool isPaused = false; // Keeps pause state between function calls

    if (HAL_GPIO_ReadPin(GPIOC, BUTTON_PIN) == GPIO_PIN_RESET) { // Assuming active low button
        HAL_Delay(50);  // Debounce delay
        if (HAL_GPIO_ReadPin(GPIOC, BUTTON_PIN) == GPIO_PIN_RESET) {
            isPaused = !isPaused;
            while (HAL_GPIO_ReadPin(GPIOC, BUTTON_PIN) == GPIO_PIN_RESET);  // Wait for button release
        }
    }

    if (!isPaused) {
        HAL_GPIO_WritePin(GPIOA, LED1_PIN, (counter & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, LED2_PIN, (counter & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, LED3_PIN, (counter & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        counter = (counter + 1) % 8;  // Increment and wrap around every 8
        HAL_Delay(1000);  // Update every second
    }
}

int main() {
    HAL_Init();  // Initialize the Hardware Abstraction Layer
    setup();
    while (1) {
        loop();
    }
}