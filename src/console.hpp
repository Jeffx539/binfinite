#pragma once
#include <stdio.h>


namespace console {
static FILE *dummy = nullptr;


void init();

template<class... Args> void log(const char *format, Args... args)
{
    printf(format, args...);
    printf("\n");
}


}// namespace console