#ifndef STRCPY_S_HEADER
#define STRCPY_S_HEADER

#include <Arduino.h>


bool strcpy_s(char* dest, int dmax, const char* src)
{
    if (!dest || !src)
        return false;
    while (dmax > 1 && *src)
    {
        *dest = *src;
        dmax--;
        src++;
        dest++;
    }
    *dest = '\0';
    return true;
}

#endif