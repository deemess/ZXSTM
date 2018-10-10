/*
 * zx.h
 *
 *  Created on: Sep 30, 2018
 *      Author: Dmitry
 */

#ifndef ZX_H_
#define ZX_H_

#include<stdint.h>

#define TESTIMAGE_LENGTH      0x00001B00

typedef struct  {
	uint8_t* rom;
	uint8_t ram[48*1024];
	uint16_t pc;
	uint16_t bordercolor;
	uint8_t flash;
	uint8_t fps;
	uint16_t fpsstr[3*25];
	uint16_t displayBuffer[320*8];//320*8
	uint16_t displayRow8;
	uint16_t displayRow8Table[24];
	uint8_t frames;
	uint32_t starttick;
	uint32_t endtick;
	uint32_t frameticks;
} ZX;

ZX* zx_init();
ZX* zx_get();



#endif /* ZX_H_ */
