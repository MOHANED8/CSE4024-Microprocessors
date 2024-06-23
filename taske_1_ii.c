#include "stm32f4xx_hal.h"

// Define the GPIO pins for the LEDs and button
#define LED1_PIN GPIO_PIN_5
#define LED2_PIN GPIO_PIN_6
#define LED3_PIN GPIO_PIN_7
#define BUTTON_PIN GPIO_PIN_13

// Timer handler declaration
TIM_HandleTypeDef htim2;
uint8_t ledIndex = 0;           // Current index for the LED pattern
uint8_t counterEnabled = 1;     // Counter enabled by default

// Function prototypes
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void setLEDs(uint8_t state);

// Error handler function
void Error_Handler(void)
{
  __disable_irq();
  while (1) { }
}

// Timer 2 initialization function
static void MX_TIM2_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    // Configure the timer
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 41999;  // Prescaler to slow down the timer
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 999;       // Set the timer period for 0.5s intervals
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();  // Handle initialization error
    }

    // Configure the clock source
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

    // Configure master/slave synchronization
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);
}

// Main function
int main(void)
{
  HAL_Init();              // Initialize the HAL Library
  SystemClock_Config();    // Configure the system clock
  MX_GPIO_Init();          // Initialize GPIO
  MX_TIM2_Init();          // Initialize Timer 2

  HAL_TIM_Base_Start_IT(&htim2);  // Start Timer 2 in interrupt mode

  while (1)
  {
    // Main loop does nothing, just waiting for interrupts
  }
}

// Timer interrupt callback function
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2 && counterEnabled)
  {
    ledIndex = (ledIndex + 1) % 5;  // Increment and wrap around every 5 counts
    setLEDs(ledIndex);              // Update LEDs based on the current index
  }
}

// GPIO initialization function
static void MX_GPIO_Init(void)
{
  // Enable GPIO clocks
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // Configure GPIO pins for LEDs
  GPIO_InitStruct.Pin = LED1_PIN | LED2_PIN | LED3_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // Push-pull output
  GPIO_InitStruct.Pull = GPIO_NOPULL;          // No pull-up or pull-down
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // Low speed
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Configure GPIO pin for Button
  GPIO_InitStruct.Pin = BUTTON_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // Interrupt on falling edge
  GPIO_InitStruct.Pull = GPIO_PULLUP;          // Pull-up resistor enabled
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  // Configure and enable EXTI line for the button
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);  // Set priority
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);          // Enable IRQ
}

// EXTI line detection callback function
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == BUTTON_PIN)
  {
    counterEnabled = !counterEnabled;  // Toggle counter enable state
  }
}

// Function to set the state of the LEDs
void setLEDs(uint8_t state)
{
  switch(state)
  {
    case 0: 
      HAL_GPIO_WritePin(GPIOA, LED1_PIN, GPIO_PIN_SET);       // Turn on LED1
      HAL_GPIO_WritePin(GPIOA, LED2_PIN | LED3_PIN, GPIO_PIN_RESET); // Turn off LED2 and LED3
      break;
    case 1:
    case 3:
      HAL_GPIO_WritePin(GPIOA, LED2_PIN, GPIO_PIN_SET);       // Turn on LED2
      HAL_GPIO_WritePin(GPIOA, LED1_PIN | LED3_PIN, GPIO_PIN_RESET); // Turn off LED1 and LED3
      break;
    case 2:
      HAL_GPIO_WritePin(GPIOA, LED3_PIN, GPIO_PIN_SET);       // Turn on LED3
      HAL_GPIO_WritePin(GPIOA, LED1_PIN | LED2_PIN, GPIO_PIN_RESET); // Turn off LED1 and LED2
      break;
    case 4:
      HAL_GPIO_WritePin(GPIOA, LED1_PIN, GPIO_PIN_SET);       // Turn on LED1
      HAL_GPIO_WritePin(GPIOA, LED2_PIN | LED3_PIN, GPIO_PIN_RESET); // Turn off LED2 and LED3
      break;
  }
}

// System clock configuration function
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  // Initialize the High-Speed Internal Oscillator (HSI)
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  // Initialize the CPU, AHB, and APB buses clocks
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}
