#include "main.h"
#include "liquidcrystal_i2c.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

/* Global Variables */
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart1;

/* Function Prototypes */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);

/* Function Definitions */
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_USART1_UART_Init();

    char rxBuffer[100] = {0};
    char temperature[10] = {0}, moisture[10] = {0};

    // Initialize the LCD
    HD44780_Init(2);
    HD44780_Clear();
    HD44780_SetCursor(0, 0);
    HD44780_PrintStr("Waiting for data");

    while (1)
    {
        // Receive data via UART
        if (HAL_UART_Receive(&huart1, (uint8_t *)rxBuffer, sizeof(rxBuffer), HAL_MAX_DELAY) == HAL_OK)
        {
            // Extract temperature and moisture from received data
        	sscanf(rxBuffer, "%*c%[^,],%s", temperature, moisture);


        }
    }
}

/* Validate Data Format */
#include <ctype.h> // For isdigit


void extractData(const char *data, char *temperature, char *moisture)
{
    sscanf(data, "%[^,],%s", temperature, moisture);
}




/* System Clock Configuration */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/* Error Handler */
void Error_Handler(void)
{
    while (1)
    {
    }
}
