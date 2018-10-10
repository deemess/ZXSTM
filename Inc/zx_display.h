/*
 * zx_display.h
 *
 *  Created on: Sep 30, 2018
 *      Author: Dmitry
 */

#ifndef ZX_DISPLAY_H_
#define ZX_DISPLAY_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "zx.h"

void zx_diplay_refresh(ZX* zx, DMA_HandleTypeDef* dma);

#endif /* ZX_DISPLAY_H_ */
