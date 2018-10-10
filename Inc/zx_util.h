/*
 * zx_util.h
 *
 *  Created on: Oct 1, 2018
 *      Author: Dmitry
 */

#ifndef ZX_UTIL_H_
#define ZX_UTIL_H_

#include <stdint.h>

typedef struct {
    uint32_t quot;
    uint8_t rem;
} divmod10_t;


char * utoa_fast_div(uint32_t value, char *buffer, uint8_t bufflen);

#endif /* ZX_UTIL_H_ */
