/*
 * zx_display.c
 *
 *  Created on: Sep 30, 2018
 *      Author: Dmitry
 */


#include "zx_display.h"
#include "LCD.h"

#define LCD_RAM              (*((volatile unsigned short *) 0x60020000)) /* RS = 1 */
#define LCD_REG              (*((volatile unsigned short *) 0x60000000)) /* RS = 0 */

//16 bit LCD - RRRRR GGGGGG BBBBB
// R = 0 - 63 (64)
// G = 0 - 127 (128)
// B = 0 - 63 (64)

const uint16_t colormap[] = {
	0b0000000000000000,//000
	0b0001000000010110,//001 0x1400B6 00010100 00000000 10110110 = 00010 000000 10110
	0b1011000000000000,//010 0xB30000 10110011 00000000 00000000 = 10110 000000 00000
	0b1011000000010110,//011 0xB400B6 10110100 00000000 10110110 = 10110 000000 10110
	0b0010110111000100,//100 0x2ABB23 00101010 10111011 00100011 = 00101 101110 00100
	0b0010110111010111,//101 0x2FBBB9 00101111 10111011 10111001 = 00101 101110 10111
	0b1011110111000100,//110 0xB8B923 10111000 10111001 00100011 = 10111 101110 00100
	0b1011110111010111,//111 0xB9B9B9 10111001 10111001 10111001 = 10111 101110 10111
};

const uint16_t colormapBright[] = {
	0b0000000000000000,//000
	0b0001100000011111,//001 0x1C00FA 00011100 00000000 11111010 = 00011 000000 11111
	0b1111000000000000,//010 0xF70000 11110111 00000000 00000000 = 11110 000000 00000
	0b1111100000011111,//011 0xF800FA 11111000 00000000 11111010 = 11111 000000 11111
	0b0011111111100110,//100 0x3AFF31 00111010 11111111 00110001 = 00111 111111 00110
	0b0100011111111111,//101 0x40FFFF 01000000 11111111 11111111 = 01000 111111 11111
	0b1111111111100110,//110 0xFDFF30 11111101 11111111 00110000 = 11111 111111 00110
	0b1111111111111111,//111 0xFFFFFF 11111111 11111111 11111111 = 11111 111111 11111
};

//Ёкранна€ область расположена в ќ«” компьютера по адресу #4000 (16384) и имеет размер 256*192 бит, 32*24 знакоместа, то есть 6144 байт
// 320 - 265 = 64 / 2 = 32 px each side horisontally
// 240 - 192 = 48 / 2 = 24 px each side vertially

/*
void zx_diplay_refresh(ZX* zx) {
	LCD_SetCursor(0, 0);
	LCD_WriteIndex(0x0022);

	for(int y=0; y<240; y++) {
		for(int x=0; x<320; x++) {

			if(x<15 && y<5) {
				LCD_WriteData(zx->fpsstr[y*15+x]);
			} else if(y<23 || y>215) {
				LCD_WriteData(zx->bordercolor);
			} else {
				if(x<31 || x>287) {
					LCD_WriteData(zx->bordercolor);
				} else {
					//screen data
					uint8_t xs = x-31;
					uint8_t ys = y-23;
					uint8_t xc = xs/8;
					uint8_t yc = ys/8;
					uint8_t colors = zx->ram[6144 + xc + yc*32];
					uint16_t ink;
					uint16_t paper;
					if(colors & 0b01000000) { //bright
						ink = colormapBright[colors & 0b00000111];
						paper = colormapBright[(colors & 0b00111000) >> 3];
					} else {
						ink = colormap[colors & 0b00000111];
						paper = colormap[(colors & 0b00111000) >> 3];
					}

					if(colors & 0b10000000 && zx->flash == 0) { //flash
						uint16_t tmp = ink;
						ink = paper;
						paper = tmp;
					}

					uint8_t screenpart =  ys/64;
					uint8_t party = ys % 64;
					uint8_t scanline = party % 8; // full scan line length = 32 * 8
					uint8_t pixels = zx->ram[scanline*32*8 + (party/8) * 32 + xc + screenpart*2048];
					uint8_t bitn = 7 - xs % 8;
					if((0b00000001 << bitn) & pixels) { //ink
						LCD_WriteData(ink);
					} else { //paper
						LCD_WriteData(paper);
					}
				}
			}
		}
	}
	zx->frames++;

}
*/

void zx_diplay_refresh(ZX* zx, DMA_HandleTypeDef* dma) {
	/*
	//set cursor
	LCD_REG	= 0x004e;
    LCD_RAM = 0;//x
    LCD_REG	= 0x004f;
    LCD_RAM = 0;//y
    */

	zx->starttick = DWT->CYCCNT;

	uint16_t row = zx->displayRow8;
	uint16_t b = zx->bordercolor;
	uint16_t buffp = 0;

	if(row == 0) {
		LCD_SetCursor(0, 0);
	    //start filling screen
		LCD_REG	= 0x0022;
		for(int y=0; y<5; y++) {
			for(int x=0; x<320; x++) {
				if(x<15) {
					//LCD_RAM = zx->fpsstr[y*15+x];
					zx->displayBuffer[buffp++] = zx->fpsstr[y*15+x];
				} else {
					//LCD_RAM = b;
					zx->displayBuffer[buffp++] = b;
				}
			}
		}
		for(int i=0; i<320*3; i++) {
			//LCD_RAM = b;
			zx->displayBuffer[buffp++] = b;
		}

	} else if(row > 2 && row < 27) {
		uint16_t displaypinit = zx->displayRow8Table[row-3];
		uint16_t displayp = displaypinit;
		uint16_t colorspinit = 6144+(row-3)*32;
		uint16_t colorsp = colorspinit;

		for(uint16_t rown=0; rown<8; rown++) {
			//32 pixels in the start
			zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b;
			zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b;
			zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b;
			zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b;
			zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b;
			zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b;
			zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b;
			zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b;

			//render 1 line
			for(uint16_t coln=0; coln<32; coln++) {
				uint8_t bitn = 0b10000000;
				uint8_t colors = zx->ram[colorsp++];
				uint8_t pixels = zx->ram[displayp++];
				uint16_t ink;
				uint16_t paper;
				if(colors & 0b01000000) { //bright
					ink = colormapBright[colors & 0b00000111];
					paper = colormapBright[(colors & 0b00111000) >> 3];
				} else {
					ink = colormap[colors & 0b00000111];
					paper = colormap[(colors & 0b00111000) >> 3];
				}
				if(colors & 0b10000000 && zx->flash == 0) { //flash
					uint16_t tmp = ink;
					ink = paper;
					paper = tmp;
				}
				if(bitn & pixels) { zx->displayBuffer[buffp++] = ink; } else { zx->displayBuffer[buffp++] = paper; }; bitn >>= 1;
				if(bitn & pixels) { zx->displayBuffer[buffp++] = ink; } else { zx->displayBuffer[buffp++] = paper; }; bitn >>= 1;
				if(bitn & pixels) { zx->displayBuffer[buffp++] = ink; } else { zx->displayBuffer[buffp++] = paper; }; bitn >>= 1;
				if(bitn & pixels) { zx->displayBuffer[buffp++] = ink; } else { zx->displayBuffer[buffp++] = paper; }; bitn >>= 1;

				if(bitn & pixels) { zx->displayBuffer[buffp++] = ink; } else { zx->displayBuffer[buffp++] = paper; }; bitn >>= 1;
				if(bitn & pixels) { zx->displayBuffer[buffp++] = ink; } else { zx->displayBuffer[buffp++] = paper; }; bitn >>= 1;
				if(bitn & pixels) { zx->displayBuffer[buffp++] = ink; } else { zx->displayBuffer[buffp++] = paper; }; bitn >>= 1;
				if(bitn & pixels) { zx->displayBuffer[buffp++] = ink; } else { zx->displayBuffer[buffp++] = paper; };

			}

			displaypinit += 256;
			displayp = displaypinit;
			colorsp = colorspinit;

			//32 pixels in the end
			zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b;
			zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b;
			zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b;
			zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b;
			zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b;
			zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b;
			zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b;
			zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b; zx->displayBuffer[buffp++] = b;
		}

	} else {
		//fill rest so far
		for(int i=0; i<320*8; i++) {
			//LCD_RAM = b;
			zx->displayBuffer[buffp++] = b;
		}
	}

	zx->displayRow8++;
	if(zx->displayRow8 > 29) {
		zx->displayRow8 = 0;
		zx->frames++;
	}

	HAL_DMA_Start_IT(dma, (uint32_t)((void*)(zx->displayBuffer)), (uint32_t)(0x60020000), 2560); //320*8

	zx->endtick = DWT->CYCCNT;
	zx->frameticks += zx->endtick - zx->starttick;

}

