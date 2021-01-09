#ifndef STRCPY_S_HEADER
#define STRCPY_S_HEADER

#include <Arduino.h>

#define ERROR_SNULLP 1
#define ERROR_SZEROLENGTH 2
#define ERROR_SOVERLAP 3
#define ERROR_SLIMITEXCEEDED 4


int strcpy_s(char* dest, int dmax, const char* src)
{
    if (dmax == 0)
        return ERROR_SZEROLENGTH;
    
    if (!dest || !src)
        return ERROR_SNULLP;
    
    int srclen = strlen(src);
    if (dest + dmax - src <= srclen || dest - src <= srclen)
        return ERROR_SOVERLAP;

    while (dmax > 1 && *src)
    {
        *dest = *src;
        dmax--;
        src++;
        dest++;

        if (dmax == 1 && *src)
        {
            *dest = '\0';
            return ERROR_SLIMITEXCEEDED;
        }

    }
    *dest = '\0';
    return 0;
}

#endif