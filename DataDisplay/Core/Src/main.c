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

			// Clear LCD and display the data
			HD44780_Clear();
			HAL_Delay(500);  // Delay for readability
			HD44780_SetCursor(0, 0);  // Line 1
			HD44780_PrintStr("Moisture: ");
			HD44780_PrintStr(moisture);
			HD44780_PrintStr("%");
			HD44780_SetCursor(0, 1);  // Line 2
			HD44780_PrintStr("Temp: ");
			HD44780_PrintStr(temperature);
			HD44780_PrintStr(" C");
			float tempValue = atof(temperature);
			float moistValue = atof(moisture);


			// Clear the buffer after processing
			memset(rxBuffer, 0, sizeof(rxBuffer));
		}
	}
}

/* Validate Data Format */
#include <ctype.h> // For isdigit


void extractData(const char *data, char *temperature, char *moisture)
{
    sscanf(data, "%[^,],%s", temperature, moisture);
}

/* USART1 Initialization Function */
static void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 9600;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_RX;  // RX only mode for receiving data
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
}


bool isValidData(const char *data)
{
    // Check if the data contains a comma
    const char *commaPosition = strchr(data, ',');
    if (commaPosition == NULL)
    {
        return false; // No comma found, invalid data
    }

    // Check if there are characters before and after the comma
    if (commaPosition == data || *(commaPosition + 1) == '\0')
    {
        return false; // Missing temperature or moisture part
    }

    // Validate the temperature part
    const char *temperaturePart = data;
    const char *moisturePart = commaPosition + 1;

    // Validate temperature (allows digits, '.', and '-')
    for (const char *ptr = temperaturePart; ptr < commaPosition; ptr++)
    {
        if (!isdigit(*ptr) && *ptr != '.' && *ptr != '-')
        {
            return false; // Invalid character in temperature
        }
    }

    // Validate moisture (allows only digits)
    for (const char *ptr = moisturePart; *ptr != '\0'; ptr++)
    {
        if (!isdigit(*ptr))
        {
            return false; // Invalid character in moisture
        }
    }

    return true; // Data is valid
}

/* I2C1 Initialization Function */
static void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;  // Set the I2C clock speed to 100kHz
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
}

/* GPIO Initialization Function */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* Configure GPIO pin for onboard LED (PA5) */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
