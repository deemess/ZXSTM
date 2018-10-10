/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "stm32f1xx_it.h"

/* USER CODE BEGIN 0 */
#include "main.h"
#include "zx.h"
#include "zx_util.h"
#include "zx_display.h"

const uint16_t smallfont5x5[] = {
		0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x0000, // 0
		0xFFFF,0x0000,0x0000,0xFFFF,0x0000,
		0xFFFF,0x0000,0x0000,0xFFFF,0x0000,
		0xFFFF,0x0000,0x0000,0xFFFF,0x0000,
		0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x0000,

		0x0000,0x0000,0x0000,0xFFFF,0x0000, // 1
		0x0000,0x0000,0x0000,0xFFFF,0x0000,
		0x0000,0x0000,0x0000,0xFFFF,0x0000,
		0x0000,0x0000,0x0000,0xFFFF,0x0000,
		0x0000,0x0000,0x0000,0xFFFF,0x0000,

		0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x0000, // 2
		0x0000,0x0000,0x0000,0xFFFF,0x0000,
		0x0000,0xFFFF,0xFFFF,0x0000,0x0000,
		0xFFFF,0x0000,0x0000,0x0000,0x0000,
		0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x0000,

		0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x0000, // 3
		0x0000,0x0000,0x0000,0xFFFF,0x0000,
		0x0000,0xFFFF,0xFFFF,0xFFFF,0x0000,
		0x0000,0x0000,0x0000,0xFFFF,0x0000,
		0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x0000,

		0xFFFF,0x0000,0x0000,0xFFFF,0x0000, // 4
		0xFFFF,0x0000,0x0000,0xFFFF,0x0000,
		0x0000,0xFFFF,0xFFFF,0xFFFF,0x0000,
		0x0000,0x0000,0x0000,0xFFFF,0x0000,
		0x0000,0x0000,0x0000,0xFFFF,0x0000,

		0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x0000, // 5
		0xFFFF,0x0000,0x0000,0x0000,0x0000,
		0x0000,0xFFFF,0xFFFF,0x0000,0x0000,
		0x0000,0x0000,0x0000,0xFFFF,0x0000,
		0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x0000,

		0x0000,0xFFFF,0xFFFF,0xFFFF,0x0000, // 6
		0xFFFF,0x0000,0x0000,0x0000,0x0000,
		0xFFFF,0xFFFF,0xFFFF,0x0000,0x0000,
		0xFFFF,0x0000,0x0000,0xFFFF,0x0000,
		0xFFFF,0xFFFF,0xFFFF,0x0000,0x0000,

		0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x0000, // 7
		0x0000,0x0000,0x0000,0xFFFF,0x0000,
		0x0000,0x0000,0xFFFF,0x0000,0x0000,
		0x0000,0xFFFF,0x0000,0x0000,0x0000,
		0x0000,0xFFFF,0x0000,0x0000,0x0000,

		0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x0000, // 8
		0xFFFF,0x0000,0x0000,0xFFFF,0x0000,
		0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x0000,
		0xFFFF,0x0000,0x0000,0xFFFF,0x0000,
		0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x0000,

		0xFFFF,0xFFFF,0xFFFF,0x0000,0x0000, // 9
		0xFFFF,0x0000,0x0000,0xFFFF,0x0000,
		0x0000,0xFFFF,0xFFFF,0xFFFF,0x0000,
		0x0000,0x0000,0x0000,0xFFFF,0x0000,
		0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x0000,

};

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_memtomem_dma1_channel1;
extern TIM_HandleTypeDef htim3;

/******************************************************************************/
/*            Cortex-M3 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
  /* USER CODE BEGIN MemoryManagement_IRQn 1 */

  /* USER CODE END MemoryManagement_IRQn 1 */
}

/**
* @brief This function handles Prefetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
  /* USER CODE BEGIN BusFault_IRQn 1 */

  /* USER CODE END BusFault_IRQn 1 */
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
  /* USER CODE BEGIN UsageFault_IRQn 1 */

  /* USER CODE END UsageFault_IRQn 1 */
}

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles DMA1 channel1 global interrupt.
*/
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_memtomem_dma1_channel1);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */
  ZX* zx = zx_get();
  zx_diplay_refresh(zx, &hdma_memtomem_dma1_channel1);

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
* @brief This function handles TIM3 global interrupt.
*/
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */

  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */

  ZX* zx = zx_get();
  zx->flash++;

  if(zx->flash > 1) {
	zx->flash = 0;
	zx->fps = zx->frames;
	zx->frames = 0;
	zx->frameticks = 0;

	char charbuff[3];
	charbuff[0]='0';
	charbuff[1]='0';
	charbuff[2]='0';

	utoa_fast_div(zx->fps, charbuff, 3);

	for(uint32_t y=0; y<5; y++) {
	  for(uint32_t x=0; x<15; x++) {
		  uint32_t digit = charbuff[x/5]-'0';
		  if(digit >= 0 && digit <= 9)
		  zx->fpsstr[y*15 + x] = smallfont5x5[digit*25 + y*5 + x%5];
	  }
	}

  }

  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  /* USER CODE END TIM3_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
