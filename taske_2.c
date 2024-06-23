#include "stm32f4xx_hal.h"
#include "main.h"

#define LED1_PIN GPIO_PIN_5           // LED1 connected to PA5
#define LED2_PIN GPIO_PIN_1           // LED2 connected to PA1
#define POTENTIOMETER_PIN GPIO_PIN_0  // Potentiometer connected to PA0
#define POTENTIOMETER_CHANNEL ADC_CHANNEL_0 // ADC Channel 0 for potentiometer

void SystemClock_Config(void);
void GPIO_Init(void);
void ADC_Init(void);
void TIM_PWM_Init(void);
void Test_LED2_PWM(void);
void Error_Handler(void);

ADC_HandleTypeDef hadc1;  // ADC handler declaration
TIM_HandleTypeDef htim2;  // Timer handler declaration

void Error_Handler(void)
{
    // User can add their own implementation to report the HAL error return state
    __disable_irq();
    while (1)
    {
    }
}

int main(void)
{
    HAL_Init();              // Initialize the HAL Library
    SystemClock_Config();    // Configure the system clock
    GPIO_Init();             // Initialize all configured peripherals (GPIO)
    ADC_Init();              // Initialize ADC
    TIM_PWM_Init();          // Initialize Timer for PWM

    // Uncomment this to test LED2 functionality independently
    // Test_LED2_PWM();

    uint32_t adcValue = 0;  // Variable to store ADC value
    uint32_t pwmValue = 0;  // Variable to store PWM value

    while (1)
    {
        HAL_ADC_Start(&hadc1); // Start ADC conversion
        if (HAL_ADC_PollForConversion(&hadc1, 1000) == HAL_OK)
        {
            adcValue = HAL_ADC_GetValue(&hadc1); // Read the ADC value (0-4095 for 12-bit ADC)
            pwmValue = (adcValue * htim2.Init.Period) / 4095;  // Scale ADC value to PWM period

            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pwmValue);  // Set PWM duty cycle for LED1
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, htim2.Init.Period - pwmValue);  // Set PWM duty cycle for LED2 inversely
        }
        HAL_ADC_Stop(&hadc1); // Stop ADC conversion
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Enable HSE Oscillator and activate PLL with HSE as source
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;            // VCO input frequency = HSE / PLLM = 1 MHz
    RCC_OscInitStruct.PLL.PLLN = 336;          // VCO output frequency = VCO input frequency * PLLN = 336 MHz
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4; // System clock frequency = VCO frequency / PLLP = 84 MHz
    RCC_OscInitStruct.PLL.PLLQ = 7;            // USB OTG FS, SDIO, RNG clock frequency = VCO frequency / PLLQ = 48 MHz

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();  // Implement error handler
    }

    // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;  // AHB = System clock = 84 MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;   // APB1 = AHB / 2 = 42 MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;   // APB2 = AHB / 1 = 84 MHz

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();  // Implement error handler
    }
}

void GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();  // Enable GPIOA clock
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // LED GPIO Configuration  
    GPIO_InitStruct.Pin = LED1_PIN | LED2_PIN;  // Use LED2_PIN (PA1) instead of GPIO_PIN_6
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // ADC GPIO Configuration  
    GPIO_InitStruct.Pin = POTENTIOMETER_PIN;    // Configure PA0 for potentiometer
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void ADC_Init(void)
{
    __HAL_RCC_ADC1_CLK_ENABLE();  // Enable ADC1 clock
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    HAL_ADC_Init(&hadc1);

    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = POTENTIOMETER_CHANNEL;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

void TIM_PWM_Init(void)
{
    __HAL_RCC_TIM2_CLK_ENABLE();  // Enable TIM2 clock
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 1000 - 1;  // Configure for 1 kHz PWM frequency
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&htim2);

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;  // Start with LED off
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);  // Configure PA5 for PWM
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);  // Configure PA1 for PWM
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);  // Start PWM on PA5
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);  // Start PWM on PA1
}

void Test_LED2_PWM(void)
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);  // Start PWM for LED2 (PA1)
    while (1)
    {
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 500);  // 50% duty cycle
        HAL_Delay(1000);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);    // 0% duty cycle
        HAL_Delay(1000);
    }
}
