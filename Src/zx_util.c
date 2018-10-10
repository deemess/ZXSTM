/*
 * zx_util.c
 *
 *  Created on: Oct 1, 2018
 *      Author: Dmitry
 */


#include "zx_util.h"

inline static divmod10_t divmodu10(uint32_t n)
{
    divmod10_t res;
// умножаем на 0.8
    res.quot = n >> 1;
    res.quot += res.quot >> 1;
    res.quot += res.quot >> 4;
    res.quot += res.quot >> 8;
    res.quot += res.quot >> 16;
    uint32_t qq = res.quot;
// делим на 8
    res.quot >>= 3;
// вычисляем остаток
    res.rem = (uint8_t)(n - ((res.quot << 1) + (qq & ~7ul)));
// корректируем остаток и частное
    if(res.rem > 9)
    {
        res.rem -= 10;
        res.quot++;
    }
    return res;
}

char * utoa_fast_div(uint32_t value, char *buffer, uint8_t bufflen)
{
	uint8_t p = bufflen-1;
    //buffer += 11;
    //*--buffer = 0;
    do
    {
        divmod10_t res = divmodu10(value);
        buffer[p--] = res.rem + '0';
        //*--buffer = res.rem + '0';
        value = res.quot;
    }
    while (value != 0 || p != 0);
    return buffer;
}
