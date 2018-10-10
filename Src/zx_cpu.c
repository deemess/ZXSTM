/*
 * zx_cpu.c
 *
 *  Created on: Oct 10, 2018
 *      Author: Dmitry
 */


#include "zx.h"
#include "zx_cpu.h"

uint8_t zx_read8(ZX* zx, uint16_t addr) {
	if(addr < 16384) {
		return zx->rom[addr];
	} else {
		return zx->ram[addr-16384];
	}
}

uint16_t zx_read16(ZX* zx, uint16_t addr) {
	if(addr < 16384) {
		return ((uint16_t*)&zx->rom[addr])[0];
	} else {
		return ((uint16_t*)&zx->ram[addr-16384])[0];
	}
}

void zx_write8(ZX* zx, uint16_t addr, uint8_t data) {
	if(addr >= 16384) {
		zx->ram[addr-16384] = data;
	}
}

void zx_write16(ZX* zx, uint16_t addr, uint16_t data) {
	if(addr >= 16384) {
		((uint16_t*)&zx->ram[addr-16384])[0] = data;
	}
}


